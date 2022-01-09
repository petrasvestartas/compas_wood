#include "StdAfx.h"
#include "RhinoUI.h"

#ifdef JOINERY_SOLVER_RHINO6
#include "joinery_solver_rhino6PlugIn.h"
#endif

#ifdef JOINERY_SOLVER_RHINO7
#include "joinery_solver_rhino7PlugIn.h"
#endif

//#include "compas.h"
#include "Rtree.h"
//#include "cgal_intersection_util.h"
//#include "cgal_vector_util.h"
#include <exception>

#include <chrono>

#pragma region command_get_centre_lines command

bool search_closest_lines_and_create_outlines(std::vector<ON_Line>& lines, std::vector<double>& radii, std::map<int, std::vector<CGAL_Polyline>>& plines) {

	double maximum_tolerance_for_cross_joint_detection = 0.15;
	double scale = 7;
	//Create RTree
	RTree<int, double, 3> tree;

	//Insert AABB
	std::vector<CGAL::Bbox_3> AABBs;
	AABBs.reserve(lines.size());

	for (int i = 0; i < lines.size(); i++) {

		plines.insert({ i, std::vector<CGAL_Polyline>() });

		//Create copy of a polyline and transform points
		CGAL_Polyline twoPolylines = {
			IK::Point_3(lines[i].from.x,lines[i].from.y,lines[i].from.z),
			IK::Point_3(lines[i].to.x,lines[i].to.y,lines[i].to.z) };

		CGAL::Bbox_3 AABB = CGAL::bbox_3(twoPolylines.begin(), twoPolylines.end(), IK());

		double t = (1 + GlobalTolerance);
		//t = 1;
		CGAL_Polyline AABB_Min_Max = {
			IK::Point_3((AABB.xmin() - radii[i] * t) , (AABB.ymin() - radii[i] * t) , (AABB.zmin() - radii[i] * t)),
			IK::Point_3((AABB.xmax() + radii[i] * t) , (AABB.ymax() + radii[i] * t) , (AABB.zmax() + radii[i] * t)),
		};

		//RhinoApp().Print("%f\n%f\n%f\n%f\n%f\n%f\n",			(AABB.xmin() - radii[i] * t), (AABB.ymin() - radii[i] * t), (AABB.zmin() - radii[i] * t), (AABB.xmax() + radii[i] * t), (AABB.ymax() + radii[i] * t), (AABB.zmax() + radii[i] * t));


		AABB = CGAL::bbox_3(AABB_Min_Max.begin(), AABB_Min_Max.end(), IK());
		AABBs.emplace_back(AABB);
		double min[3] = { AABB.xmin(), AABB.ymin(), AABB.zmin() };
		double max[3] = { AABB.xmax(), AABB.ymax(), AABB.zmax() };
		tree.Insert(min, max, i);
	}

	//Search Tree
	int collision_count = 0;
	for (int i = 0; i < lines.size(); i++) {//AABB.size()

		//Get lines points
		IK::Point_3 p0(lines[i].from.x, lines[i].from.y, lines[i].from.z);
		IK::Point_3 p1(lines[i].to.x, lines[i].to.y, lines[i].to.z);
		IK::Segment_3 s0(p0, p1);


		//std::vector<int> result;
		auto callback = [&i, &lines, &s0, &radii, &plines, &maximum_tolerance_for_cross_joint_detection, &scale](int foundValue) -> bool
		{
			//RhinoApp().Print("%i %i \n", i, foundValue);

			//if (i < foundValue && CGAL_BoxUtil::GetCollision(elements[i].oob, elements[foundValue].oob)) {
			if (i < foundValue) {

				//RhinoApp().Print("aaaaaa %i %i \n", i, foundValue);

				IK::Segment_3 s1(
					IK::Point_3(lines[foundValue].from.x, lines[foundValue].from.y, lines[foundValue].from.z),
					IK::Point_3(lines[foundValue].to.x, lines[foundValue].to.y, lines[foundValue].to.z)
				);


				//Intersect two lines
				IK::Point_3 p;
				cgal_intersection_util::LineLine3D(s0, s1, p);
				IK::Point_3 cp0 = s0.supporting_line().projection(p);
				IK::Point_3 cp1 = s1.supporting_line().projection(p);



				double t0, t1;
				cgal_intersection_util::ClosestPointTo(p, s0, t0);
				cgal_intersection_util::ClosestPointTo(p, s1, t1);

				//RhinoApp().Print("%f %f \n", t0, t1);

				if ((t0 >= (0 + maximum_tolerance_for_cross_joint_detection) && t0 <= 1 - maximum_tolerance_for_cross_joint_detection) && (t1 >= 0 + maximum_tolerance_for_cross_joint_detection && t1 <= 1 - maximum_tolerance_for_cross_joint_detection)) {

					IK::Vector_3 d0 = s0.supporting_line().to_vector();
					cgal_vector_util::Unitize(d0);
					double distance0 = radii[i] * scale;
					d0 *= distance0;

					IK::Vector_3 d1 = s1.supporting_line().to_vector();
					cgal_vector_util::Unitize(d1);
					double distance1 = radii[foundValue] * scale;
					d1 *= distance1;

					IK::Vector_3 cross = CGAL::cross_product(s0.to_vector(), s1.to_vector());
					cgal_vector_util::Unitize(cross);


					//Pair of polygon0
					IK::Vector_3 cross0 = cross * radii[i];
					IK::Vector_3 offset0 = CGAL::cross_product(cross0, d0);
					cgal_vector_util::Unitize(offset0);
					offset0 *= radii[i];

					CGAL_Polyline pline0_0 = {
						cp0 - d0 - cross0 - offset0,
						cp0 + d0 - cross0 - offset0,
						cp0 + d0 + cross0 - offset0,
						cp0 - d0 + cross0 - offset0,
						cp0 - d0 - cross0 - offset0,
					};


					CGAL_Polyline pline0_1 = {
						cp0 - d0 - cross0 + offset0,
						cp0 + d0 - cross0 + offset0,
						cp0 + d0 + cross0 + offset0,
						cp0 - d0 + cross0 + offset0,
						cp0 - d0 - cross0 + offset0,
					};

					//Pair of polygon1
					IK::Vector_3 cross1 = cross * radii[foundValue];
					IK::Vector_3 offset1 = CGAL::cross_product(cross1, d1);
					cgal_vector_util::Unitize(offset1);
					offset1 *= radii[foundValue];

					CGAL_Polyline pline1_0 = {
						cp1 - d1 - cross1 - offset1,
						cp1 + d1 - cross1 - offset1,
						cp1 + d1 + cross1 - offset1,
						cp1 - d1 + cross1 - offset1,
						cp1 - d1 - cross1 - offset1
					};

					CGAL_Polyline pline1_1 = {
						cp1 - d1 - cross1 + offset1,
						cp1 + d1 - cross1 + offset1,
						cp1 + d1 + cross1 + offset1,
						cp1 - d1 + cross1 + offset1,
						cp1 - d1 - cross1 + offset1
					};

					plines[i].emplace_back(pline0_0);
					plines[i].emplace_back(pline0_1);
					plines[foundValue].emplace_back(pline1_0);
					plines[foundValue].emplace_back(pline1_1);


					return true;
				}
				return true;
			}


			return true;
		};



		double min[3] = { AABBs[i].xmin(), AABBs[i].ymin(), AABBs[i].zmin() };
		double max[3] = { AABBs[i].xmax(), AABBs[i].ymax(), AABBs[i].zmax() };



		int nhits = tree.Search(min, max, callback);//callback in this method call callback above

	}

	//Output
	return true;
}

static int RhCopyBrepFaceLoopCurves(const ON_Brep* brep, ON_SimpleArray<ON_Curve*>& out_curves, bool only_curved = true) {

	if (NULL == brep)
		return 0;

	const int out_curves_Count = out_curves.Count();

	for (int fi = 0; fi < brep->m_F.Count(); fi++) {
		const ON_BrepFace& face = brep->m_F[fi];

		if (only_curved)
			if (!face.IsPlanar())continue;

		for (int fli = 0; fli < face.LoopCount(); fli++) {
			ON_BrepLoop* loop = face.Loop(fli);
			if (NULL == loop)
				continue;

			for (int lti = 0; lti < loop->TrimCount(); lti++) {
				ON_BrepTrim* trim = loop->Trim(lti);
				if (NULL == trim)
					continue;

				// This will be NULL for singular trims.
				ON_BrepEdge* edge = trim->Edge();
				if (NULL == edge)
					continue;

				ON_Curve* crv = edge->DuplicateCurve();
				if (NULL == crv)
					continue;

				// true if the 2d trim and 3d edge have opposite orientations.
				if (trim->m_bRev3d)
					crv->Reverse();

				// true if face orientation is opposite of natural surface orientation
				if (face.m_bRev)
					crv->Reverse();

				out_curves.Append(crv);
			}
		}
	}

	return out_curves.Count() - out_curves_Count;
}

bool IsArc(const ON_Curve* crv, double tol, bool& bCircle, ON_3dPoint& center) {
	if (0 == crv)
		return false;

	// Is the curve an arc curve?
	const ON_ArcCurve* arc_crv = ON_ArcCurve::Cast(crv);
	if (arc_crv) {
		bCircle = arc_crv->IsCircle();
		return true;
	}

	// Is the curve a polycurve that looks like an arc?
	const ON_PolyCurve* poly_crv = ON_PolyCurve::Cast(crv);
	if (poly_crv) {
		ON_Arc arc;
		if (poly_crv->IsArc(0, &arc, tol)) {
			bCircle = arc.IsCircle();
			return true;
		}
	}

	// Is the curve an ellipse that looks like an arc?
	ON_Ellipse ellipse;
	if (crv->IsEllipse(0, &ellipse, tol)) {
		if (ellipse.IsCircle()) {
			bCircle = crv->IsClosed() ? true : false;
			center = ellipse.Center();
			return true;
		}
	}

	// Is the curve a NURBS curve that looks like an arc?
	const ON_NurbsCurve* nurb_crv = ON_NurbsCurve::Cast(crv);
	if (nurb_crv) {
		ON_Arc arc;
		if (nurb_crv->IsArc(0, &arc, tol)) {
			bCircle = arc.IsCircle();
			return true;
		}
	}

	return false;
}

bool pipes_to_lines(std::vector<ON_Brep*>& pipes, std::vector<ON_Line>& lines, std::vector<double>& radii, double tol, const CRhinoCommandContext& context) {

	lines.reserve(pipes.size());
	radii.reserve(pipes.size());


	for (int i = 0; i < pipes.size(); i++) {

		for (int fi = 0; fi < pipes[i]->m_F.Count(); fi++) {
			const ON_BrepFace& face = pipes[i]->m_F[fi];


			ON_Cylinder cylinder;
			if (face.IsCylinder(&cylinder, tol)) {

				//to xy
				ON_Xform to_xy;
				ON_Plane plane_xy(ON_3dPoint(0, 0, 0), ON_3dVector(0, 0, 1));
				to_xy.Rotation(cylinder.circle.plane, plane_xy);

				ON_Xform to_xy_inv;
				to_xy_inv.Rotation(plane_xy, cylinder.circle.plane);

				//copy brean and transform
				auto brep = ON_Brep(*face.Brep());
				brep.Transform(to_xy);

				//bbox
				auto bbox = ON_BoundingBox::EmptyBoundingBox;
				brep.GetTightBoundingBox(bbox);
				ON_3dPoint center = bbox.Center();
				double length = bbox.m_max.z - bbox.m_min.z;

				//Create planes and transform back to 3d
				ON_Plane plane_0(ON_3dPoint(center.x, center.y, bbox.m_min.z), ON_3dVector(0, 0, 1));
				ON_Plane plane_1(ON_3dPoint(center.x, center.y, bbox.m_max.z), ON_3dVector(0, 0, 1));
				plane_0.Transform(to_xy_inv);
				plane_1.Transform(to_xy_inv);

				//Output
				ON_Line line(plane_0.origin, plane_1.origin);
				lines.emplace_back(line);
				radii.emplace_back(cylinder.circle.radius);



			}
		}


	}
	return lines.size() > 0;
}


bool UI(const CRhinoCommandContext& context, std::vector<ON_Brep*>& pipes) {

	/////////////////////////////////////////////////////////////////////
	//Polylines are grouped
	/////////////////////////////////////////////////////////////////////
	CRhinoGetObject go;
	go.SetCommandPrompt(L"Select Pipes");
	go.SetGeometryFilter(CRhinoGetObject::polysrf_object);
	go.EnableGroupSelect(TRUE);
	go.EnableSubObjectSelect(FALSE);
	go.GetObjects(1, 0);
	if (go.CommandResult() != CRhinoCommand::success) return false;



	pipes.reserve(go.ObjectCount());

	for (int i = 0; i < go.ObjectCount(); i++) {

		//Convert Curve to Polyline
		const CRhinoObjRef& o = go.Object(i);
		const  ON_Brep* g = o.Brep();
		if (!g) continue;
		if (!g->IsSolid())continue;
		pipes.emplace_back(g->Duplicate());
	}

	if (pipes.size() == 0) return false;


	/////////////////////////////////////////////////////////////////////
	//Output
	/////////////////////////////////////////////////////////////////////
	return true;



}



class command_get_centre_lines : public CRhinoCommand {
public:

	command_get_centre_lines() = default;
	~command_get_centre_lines() = default;

	UUID CommandUUID() override {
		static const GUID joinery_solver_rhino7Command_UUID =
		{ 0xb6d77b51, 0xcdd, 0x4002, { 0xbf, 0x15, 0x42, 0x2f, 0xa6, 0xe2, 0xce, 0xcc } };
		return joinery_solver_rhino7Command_UUID;
	}


	const wchar_t* EnglishCommandName() override { return L"joinery_solver_get_centre_lines"; }
	CRhinoCommand::result RunCommand(const CRhinoCommandContext& context) override;
};


static class command_get_centre_lines thejoinery_solver_rhino7Command;



CRhinoCommand::result command_get_centre_lines::RunCommand(const CRhinoCommandContext& context) {

	/////////////////////////////////////////////////////////////////////
	//Input
	/////////////////////////////////////////////////////////////////////
	std::vector<ON_Brep*> pipes;

	if (!UI(context, pipes)) return CRhinoCommand::failure;
	RhinoApp().Print("Number of pipes %i \n", pipes.size());

	/////////////////////////////////////////////////////////////////////
	//Main Method
	/////////////////////////////////////////////////////////////////////
	RhinoApp().Print("============================================================================== CPP + \n");
	auto start = std::chrono::high_resolution_clock::now();

	std::vector<ON_Line> lines;
	std::vector<double> radii;
	double tol = context.m_doc.AbsoluteTolerance();
	if (!pipes_to_lines(pipes, lines, radii, tol, context))  return CRhinoCommand::failure;
	RhinoApp().Print("Number of lines %i \n", lines.size());

	std::map<int, std::vector<CGAL_Polyline>>plines;
	search_closest_lines_and_create_outlines(lines, radii, plines);
	RhinoApp().Print("Number of plines %i \n", plines.size());


	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	RhinoApp().Print("==================================== %d ms ==================================== \n", duration);
	RhinoApp().Print("============================================================================== CPP - \n");

	/////////////////////////////////////////////////////////////////////
	//Output
	/////////////////////////////////////////////////////////////////////

	//RhinoApp().Print(L"Input %d \n", lines.size());

	/////////////////////////////////////////////////////////////////////
	//Create layer
	/////////////////////////////////////////////////////////////////////
	const wchar_t* layer_name = L"joinery_solver_pipe_lines";
	int layer_index = RhinoFindOrCreateLayer(context.m_doc, layer_name);
	if (layer_index == ON_UNSET_INT_INDEX)
		return CRhinoCommand::failure;

	ON_3dmObjectAttributes attributes;
	context.m_doc.GetDefaultObjectAttributes(attributes);
	attributes.m_layer_index = layer_index;

	/////////////////////////////////////////////////////////////////////
	//Add elements and group them
	/////////////////////////////////////////////////////////////////////
	ON_SimpleArray<const CRhinoObject*> group_members(lines.size());

	int count = 0;
	for (auto& output : lines) {


		// Attach user string to object's attributes
		CRhinoCurveObject* curve_object = context.m_doc.AddCurveObject(output, &attributes);

		// Attach user string to object's attributes
		CRhinoObjectAttributes attribs = curve_object->Attributes();
		ON_wString key(L"radius");
		ON_wString text(L"%f", radii[count]);
		attribs.SetUserString(key, text);
		context.m_doc.ModifyObjectAttributes(curve_object, attribs);

		//Add to group
		group_members.Append(curve_object);
		count++;
	}

	int group_index = context.m_doc.m_group_table.AddGroup(ON_Group(), group_members);
	if (group_index < 0) CRhinoCommand::failure;


	/////////////////////////////////////////////////////////////////////
	//Add Polylines
	/////////////////////////////////////////////////////////////////////

	for (auto const& x : plines) {

		ON_SimpleArray<const CRhinoObject*> group_members_pairs(x.second.size());
		for (int i = 0; i < x.second.size(); i++) {


			count = 0;
			//for (int j = 0; j < 2; j++) {

				// Attach user string to object's attributes
			ON_Polyline polyline;
			polyline.Reserve(x.second[i].size());
			for (auto& pt : x.second[i])
				polyline.Append(ON_3dPoint(pt.hx(), pt.hy(), pt.hz()));

			CRhinoCurveObject* curve_object = context.m_doc.AddCurveObject(polyline, &attributes);

			// Attach user string to object's attributes
			CRhinoObjectAttributes attribs = curve_object->Attributes();
			ON_wString key(L"radius");
			ON_wString text(L"%f", radii[count]);
			attribs.SetUserString(key, text);
			context.m_doc.ModifyObjectAttributes(curve_object, attribs);

			//Add to group
			group_members_pairs.Append(curve_object);
			count++;
			//}

		}

		int group_index_pairs = context.m_doc.m_group_table.AddGroup(ON_Group(), group_members_pairs);
		if (group_index_pairs < 0) CRhinoCommand::failure;

	}
	context.m_doc.Redraw();



	return CRhinoCommand::success;
}

#pragma endregion

