#include "StdAfx.h"
#include "RhinoUI.h"

#ifdef JOINERY_SOLVER_RHINO6
#include "joinery_solver_rhino6PlugIn.h"
#endif

#ifdef JOINERY_SOLVER_RHINO7
#include "joinery_solver_rhino7PlugIn.h"
#endif

//#include "compas.h"
#include <exception>

#include <chrono>

#pragma region command_group_loft command


bool UI(const CRhinoCommandContext& context, std::vector<ON_Curve*>& input_polyline_pairs, std::vector<int>& group_indices) {

	/////////////////////////////////////////////////////////////////////
	//Polylines are grouped
	/////////////////////////////////////////////////////////////////////
	CRhinoGetObject go;
	go.SetCommandPrompt(L"Select pairs of polylines");
	go.SetGeometryFilter(CRhinoGetObject::curve_object);
	go.EnableGroupSelect(TRUE);
	go.EnableSubObjectSelect(FALSE);
	go.GetObjects(1, 0);
	if (go.CommandResult() != CRhinoCommand::success) return false;



	input_polyline_pairs.reserve(go.ObjectCount() - go.ObjectCount() % 2);


	group_indices.reserve(go.ObjectCount());
	for (int i = 0; i < go.ObjectCount() - go.ObjectCount() % 2; i++) {

		//Convert Curve to Polyline
		const CRhinoObjRef& obj_ref = go.Object(i);
		const CRhinoObject* obj = obj_ref.Object();

		const  ON_Curve* crv = obj_ref.Curve();
		if (!crv) continue;
		if (!crv->IsClosed())continue;
		ON_Curve* curve = crv->DuplicateCurve();
		input_polyline_pairs.emplace_back(curve);

		if (nullptr != obj) {
			group_indices.emplace_back(obj->Attributes().TopGroup());
		}
	}

	if (input_polyline_pairs.size() == 0 || input_polyline_pairs.size() % 2 == 1) return false;


	/////////////////////////////////////////////////////////////////////
	//Output
	/////////////////////////////////////////////////////////////////////
	return true;



}



class command_group_loft : public CRhinoCommand {
public:

	command_group_loft() = default;
	~command_group_loft() = default;

	UUID CommandUUID() override {
		static const GUID joinery_solver_rhino7Command_UUID =
		{ 0x9a0726ab, 0x42a8, 0x4aa5, { 0x85, 0x37, 0x6d, 0x48, 0x55, 0x70, 0xed, 0x9b } };
		return joinery_solver_rhino7Command_UUID;
	}


	const wchar_t* EnglishCommandName() override { return L"joinery_solver_group_loft"; }
	CRhinoCommand::result RunCommand(const CRhinoCommandContext& context) override;
};

static class command_group_loft thejoinery_solver_rhino7Command;



CRhinoCommand::result command_group_loft::RunCommand(const CRhinoCommandContext& context) {

	/////////////////////////////////////////////////////////////////////
	//Input
	/////////////////////////////////////////////////////////////////////
	std::vector<ON_Curve*> input_polyline_pairs;
	std::vector<int> group_indices;
	int search_type = 2;
	double division_distance = 1000;
	double shift = 0.5;
	if (!UI(context, input_polyline_pairs, group_indices)) return CRhinoCommand::failure;


	/////////////////////////////////////////////////////////////////////
	//Main Method
	/////////////////////////////////////////////////////////////////////
	RhinoApp().Print("============================================================================== CPP + \n");
	auto start = std::chrono::high_resolution_clock::now();


	std::map<int, ON_SimpleArray<const CRhinoObject*>> groups;

	bool grouping = group_indices.size() == input_polyline_pairs.size();
	RhinoApp().Print(L"Grouping %i %i %i \n", grouping, group_indices.size(), (int)(input_polyline_pairs.size() * 0.5));
	if (grouping) {
		for (int i = 0; i < group_indices.size(); i += 2) {
			if (!groups.count(group_indices[i])) {
				groups.insert({ group_indices[i] , ON_SimpleArray<const CRhinoObject*>() });
			}
		}

	}


	for (int i = 0; i < input_polyline_pairs.size(); i += 2) {


		CArgsRhinoLoft args;
		args.m_loftcurves.SetCapacity(2);

		// Add curves to loft arguments object
		// New loft curve
		CRhinoLoftCurve* lc0 = new CRhinoLoftCurve;
		CRhinoLoftCurve* lc1 = new CRhinoLoftCurve;

		// Duplicate the selected curve. Note,
		// the loft curve will delete this curve.
		lc0->m_curve = input_polyline_pairs[i]->DuplicateCurve();
		lc1->m_curve = input_polyline_pairs[i + 1]->DuplicateCurve();
		lc0->m_curve->RemoveShortSegments(ON_ZERO_TOLERANCE);
		lc1->m_curve->RemoveShortSegments(ON_ZERO_TOLERANCE);

		// Set other loft curve parameters
		lc0->m_bPlanar = (lc0->m_curve->IsPlanar(&lc0->m_plane) ? true : false);
		lc1->m_bPlanar = (lc1->m_curve->IsPlanar(&lc1->m_plane) ? true : false);


		// Append loft curve to loft argument
		args.m_loftcurves.Append(lc0);
		args.m_loftcurves.Append(lc1);

		// If the starting loft curve has a trim,
		// set the start condition to tangent.
		if (args.m_loftcurves[0] && args.m_loftcurves[0]->m_trim) {
			args.m_start_condition = CArgsRhinoLoft::leTangent;
			args.m_bAllowStartTangent = TRUE;
		}

		// If the ending loft curve has a trim,
		// set the end condition to tangent.
		if (args.m_loftcurves[1] && args.m_loftcurves[1]->m_trim) {
			args.m_end_condition = CArgsRhinoLoft::leTangent;
			args.m_bAllowEndTangent = TRUE;
		}

		// Do the loft calculation
		ON_SimpleArray<ON_NurbsSurface*> srf_list;
		bool rc = RhinoSdkLoftSurface(args, srf_list);

		// Delete the loft curves so we do not leak memory.
		for (int j = 0; j < args.m_loftcurves.Count(); j++)
			delete args.m_loftcurves[j];
		args.m_loftcurves.Empty();

		// If the loft operation failed, bail.
		if (!rc)
			return failure;


		// If only one surface was calculated, add it to Rhino
		if (srf_list.Count() == 1) {

			ON_Brep* brep = ON_Brep::New();
			brep->NewFace(*srf_list[0]);



			brep = RhinoBrepCapPlanarHoles(brep, 0.01);
			if (brep) {
				CRhinoBrepObject* o = context.m_doc.AddBrepObject(*brep);

				if (grouping)
					groups[group_indices[i]].Append(o);

				delete brep;
			} else {
				CRhinoSurfaceObject* o = context.m_doc.AddSurfaceObject(*srf_list[0]);

				if (grouping)
					groups[group_indices[i]].Append(o);
			}



			// CRhinoDoc::AddSurfaceObject() make a copy.
			// So, delete original so memory is not leaked.
			delete srf_list[0];
		} else {
			// If more than one surface was calculated,
			// create a list of breps.
			ON_SimpleArray<ON_Brep*> brep_list;
			for (int j = 0; j < srf_list.Count(); j++) {
				if (srf_list[j]->IsValid()) {
					ON_Brep* brep = ON_Brep::New();
					brep->NewFace(*srf_list[j]);

					// ON_Brep::NewFace() make a copy.
					// So, delete original so memory is not leaked.
					delete srf_list[j];

					brep_list.Append(brep);
				}
			}

			// Try joining all breps
			double tol = context.m_doc.AbsoluteTolerance();

			ON_Brep* brep = RhinoJoinBreps(brep_list, tol);


			if (brep) {
				ON_Brep* brep2 = RhinoBrepCapPlanarHoles(brep, 0.01);
				if (brep2) {
					CRhinoBrepObject* o = context.m_doc.AddBrepObject(*brep2);
					if (grouping)
						groups[group_indices[i]].Append(o);
				} else {
					CRhinoBrepObject* o = context.m_doc.AddBrepObject(*brep);
					if (grouping)
						groups[group_indices[i]].Append(o);
				}

				// CRhinoDoc::AddBrepObject() make a copy.
				// So, delete original so memory is not leaked.
				delete brep;
				delete brep2;
			} else {
				// Othewise just add the individual breps to Rhino.
				for (int j = 0; j < brep_list.Count(); j++) {
					if (brep_list[j]) {
						//brep_list[j] = RhinoBrepCapPlanarHoles(brep_list[j], 0.01);
						CRhinoBrepObject* o = context.m_doc.AddBrepObject(*brep_list[j]);
						if (grouping)
							groups[group_indices[i]].Append(o);
						// CRhinoDoc::AddBrepObject() make a copy.
						// So, delete original so memory is not leaked.
						delete brep_list[j];
					}
				}
			}
		}


	}



	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	RhinoApp().Print("==================================== %d ms ==================================== \n", duration);
	RhinoApp().Print("============================================================================== CPP - \n");

	/////////////////////////////////////////////////////////////////////
	//Output
	/////////////////////////////////////////////////////////////////////

	RhinoApp().Print(L"Input %d \n", input_polyline_pairs.size());

	//Create layer
	const wchar_t* layer_name = L"joinery_solver_cuts";
	int layer_index = RhinoFindOrCreateLayer(context.m_doc, layer_name);
	if (layer_index == ON_UNSET_INT_INDEX)
		return CRhinoCommand::failure;

	ON_3dmObjectAttributes attributes;
	context.m_doc.GetDefaultObjectAttributes(attributes);
	attributes.m_layer_index = layer_index;


	if (grouping) {
		for (auto& pairs : groups) {
			int group_index = context.m_doc.m_group_table.AddGroup(ON_Group(), pairs.second);
		}
	}

	context.m_doc.Redraw();



	return CRhinoCommand::success;
}

#pragma endregion

