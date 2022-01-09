#include "StdAfx.h"
#include "RhinoUI.h"

#ifdef JOINERY_SOLVER_RHINO6
#include "joinery_solver_rhino6PlugIn.h"
#endif

#ifdef JOINERY_SOLVER_RHINO7
#include "joinery_solver_rhino7PlugIn.h"
#endif

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Polygon_mesh_processing/corefinement.h>
typedef CGAL::Exact_predicates_inexact_constructions_kernel  IK;

#include <chrono>


#pragma region command_mesh_difference command



CGAL::Surface_mesh<CGAL::Exact_predicates_inexact_constructions_kernel::Point_3> Convert_RhinoMeshToCGAL(const ON_Mesh* meshRhino0_) {



	ON_Mesh* meshRhino0 = RhinoWeldMesh(*meshRhino0_, ON_PI);
	meshRhino0->ConvertQuadsToTriangles();
	meshRhino0->CombineIdenticalVertices(true, true);
	meshRhino0->CullUnusedVertices();
	meshRhino0->Cleanup(true);
	meshRhino0->CullDegenerateFaces();
	meshRhino0->CullUnusedVertices();

	CGAL::Surface_mesh<CGAL::Exact_predicates_inexact_constructions_kernel::Point_3> mesh1; //typedef CGAL::Surface_mesh<K::Point_3>             Mesh; typedef CGAL::Exact_predicates_inexact_constructions_kernel K;


	for (int i = 0; i < meshRhino0->m_V.Count(); i++) {
		auto x = meshRhino0->m_V[i].x;
		auto y = meshRhino0->m_V[i].y;
		auto z = meshRhino0->m_V[i].z;
		auto v = CGAL::Epick::Point_3(x, y, z);
		mesh1.add_vertex(v);
	}

	for (int i = 0; i < meshRhino0->m_F.Count(); i++) {
		auto x = meshRhino0->m_F[i].vi[0];
		auto y = meshRhino0->m_F[i].vi[1];
		auto z = meshRhino0->m_F[i].vi[2];
		mesh1.add_face(CGAL::SM_Vertex_index(x), CGAL::SM_Vertex_index(y), CGAL::SM_Vertex_index(z));
	}

	delete meshRhino0;

	return mesh1;

}

////https://stackoverflow.com/questions/46808246/cgal-get-face-data-from-surface-mesh

ON_Mesh Convert_CGALMeshToRhino(CGAL::Surface_mesh<CGAL::Exact_predicates_inexact_constructions_kernel::Point_3>  out) {

	ON_Mesh meshRebuilt(out.num_faces(), out.number_of_vertices(), false, false);

	for (auto vi : out.vertices()) {
		auto pt = out.point(vi);
		meshRebuilt.SetVertex(vi.idx(), ON_3dPoint(pt.x(), pt.y(), pt.z()));
	}

	//Get face indices ...

	for (auto face_index : out.faces()) {
		std::vector<uint32_t> indices;
		CGAL::Vertex_around_face_circulator<CGAL::Surface_mesh<CGAL::Exact_predicates_inexact_constructions_kernel::Point_3>> vcirc(out.halfedge(face_index), out), done(vcirc);
		do indices.push_back(*vcirc++); while (vcirc != done);
		meshRebuilt.SetTriangle(face_index.idx(), indices[0], indices[1], indices[2]);

	}

	return meshRebuilt;

}

struct Visitor :
	public CGAL::Polygon_mesh_processing::Corefinement::Default_visitor<CGAL::Surface_mesh<IK::Point_3>> {
	typedef CGAL::Surface_mesh<IK::Point_3>::Face_index face_descriptor;

	boost::container::flat_map<const CGAL::Surface_mesh<IK::Point_3>*, CGAL::Surface_mesh<IK::Point_3>::Property_map<CGAL::Surface_mesh<IK::Point_3>::Face_index, int> > properties;
	int face_id;

	Visitor() {
		properties.reserve(3);
		face_id = -1;
	}

	// visitor API overloaded
	void before_subface_creations(face_descriptor f_split, CGAL::Surface_mesh<IK::Point_3>& tm) {
		face_id = properties[&tm][f_split];
	}

	void after_subface_created(face_descriptor f_new, CGAL::Surface_mesh<IK::Point_3>& tm) {
		properties[&tm][f_new] = face_id;
	}

	void after_face_copy(face_descriptor f_src, CGAL::Surface_mesh<IK::Point_3>& tm_src,
		face_descriptor f_tgt, CGAL::Surface_mesh<IK::Point_3>& tm_tgt) {
		properties[&tm_tgt][f_tgt] = properties[&tm_src][f_src];
	}
};

//To track colors
//https://github.com/CGAL/cgal/blob/master/Polygon_mesh_processing/examples/Polygon_mesh_processing/corefinement_mesh_union_with_attributes.cpp
bool MeshBoolean_CreateArrayTrackColors(

	//CGAL::Surface_mesh<IK::Point_3>& meshList,
	std::vector<CGAL::Surface_mesh<IK::Point_3>>& meshList,
	CGAL::Surface_mesh<IK::Point_3>& output,
	int Difference_Union_Intersection = 0


) {


	//////////////////////////////////////////////////////////////////
	//// Perform CGAL Boolean
	//////////////////////////////////////////////////////////////////


	CGAL::Surface_mesh<CGAL::Exact_predicates_inexact_constructions_kernel::Point_3>& lastMesh = meshList[0];
	CGAL::Surface_mesh<IK::Point_3>::Property_map<CGAL::Surface_mesh<IK::Point_3>::Face_index, int>  lastMesh_id = lastMesh.add_property_map<CGAL::Surface_mesh<IK::Point_3>::Face_index, int>("f:id", -1).first;
	for (CGAL::Surface_mesh<IK::Point_3>::Face_index f : faces(lastMesh))
		lastMesh_id[f] = 1;


	for (int i = 1; i < meshList.size(); i++) {//meshList.size ()

		Visitor visitor;

		//Properties last mesh
		//Mesh::Property_map<Mesh::Face_index, int>  lastMesh_id = lastMesh.add_property_map<Mesh::Face_index, int> ("f:id", -1).first;
		//for ( Mesh::Face_index f : faces (lastMesh) )
		//    lastMesh_id[f] = 1;
		visitor.properties[&lastMesh] = lastMesh_id;//From previous iteration must or must not contain property map?

		////Properties current mesh
		CGAL::Surface_mesh<IK::Point_3>::Property_map<CGAL::Surface_mesh<IK::Point_3>::Face_index, int>  mesh_id = meshList[i].add_property_map<CGAL::Surface_mesh<IK::Point_3>::Face_index, int>("f:id", -1).first;
		for (CGAL::Surface_mesh<IK::Point_3>::Face_index f : faces(meshList[i]))
			mesh_id[f] = (i + 1);
		visitor.properties[&meshList[i]] = mesh_id;

		////Properties Out
		CGAL::Surface_mesh<IK::Point_3>   out;
		CGAL::Surface_mesh<IK::Point_3>::Property_map<CGAL::Surface_mesh<IK::Point_3>::Face_index, int>  out_id = out.add_property_map<CGAL::Surface_mesh<IK::Point_3>::Face_index, int>("f:id", -1).first;
		visitor.properties[&out] = out_id;

		bool valid_union = false;
		const bool throw_on_self_intersection = true;

		try {

			if (Difference_Union_Intersection == 1) {
				valid_union = CGAL::Polygon_mesh_processing::corefine_and_compute_union(lastMesh, meshList[i], out, CGAL::Polygon_mesh_processing::parameters::visitor(visitor), CGAL::Polygon_mesh_processing::parameters::throw_on_self_intersection(true));//, , CGAL::Polygon_mesh_processing::parameters::throw_on_self_intersection (true) , CGAL::Polygon_mesh_processing::parameters::visitor (visitor)
			} else if (Difference_Union_Intersection == 2) {
				valid_union = CGAL::Polygon_mesh_processing::corefine_and_compute_intersection(lastMesh, meshList[i], out, CGAL::Polygon_mesh_processing::parameters::visitor(visitor), CGAL::Polygon_mesh_processing::parameters::throw_on_self_intersection(true)); //, CGAL::Polygon_mesh_processing::parameters::visitor (visitor)
			} else {
				valid_union = CGAL::Polygon_mesh_processing::corefine_and_compute_difference(lastMesh, meshList[i], out, CGAL::Polygon_mesh_processing::parameters::visitor(visitor), CGAL::Polygon_mesh_processing::parameters::throw_on_self_intersection(true));//, CGAL::Polygon_mesh_processing::parameters::visitor (visitor)
			}

			//std::ofstream myfile3;
			//myfile3.open ("C:\\libs\\PInvokeCPPCSHARP\\PInvoke\\x64\\Release\\TrackFaceIDEnd.txt");
			//for ( Mesh::Face_index f : faces (out) )
			//    myfile3 << f << " ID" << out_id[f] << " \n";
			//myfile3.close ();

			lastMesh = valid_union ? out : meshList[i];

			//for ( Mesh::Face_index f : faces (out) )
			//    lastMesh_id[f] = out_id[f];

			lastMesh_id = lastMesh.add_property_map<CGAL::Surface_mesh<IK::Point_3>::Face_index, int>("f:id", -1).first;
			for (CGAL::Surface_mesh<IK::Point_3>::Face_index f : faces(out)) {
				auto faceID = out_id[f];
				lastMesh_id[f] = faceID;
			}



		} catch (const std::exception& e) {
			lastMesh = meshList[i];

		}

	}

	output = lastMesh;

	return true;


}

//To track colors
//https://github.com/CGAL/cgal/blob/master/Polygon_mesh_processing/examples/Polygon_mesh_processing/corefinement_mesh_union_with_attributes.cpp
bool MeshBoolean_CreateArray(


	std::vector<CGAL::Surface_mesh<IK::Point_3>>& meshList,
	CGAL::Surface_mesh<IK::Point_3>& output,
	int Difference_Union_Intersection = 0

) {



	//////////////////////////////////////////////////////////////////
	//// Perform CGAL Boolean
	//////////////////////////////////////////////////////////////////

	CGAL::Surface_mesh<CGAL::Exact_predicates_inexact_constructions_kernel::Point_3>  lastMesh = meshList[0];
	for (int i = 1; i < meshList.size(); i++) {//meshList.size ()

		bool valid_union = false;
		const bool throw_on_self_intersection = true;
		//const bool NamedParameters1(1) = ;
	  // const throw_on_self_intersection (true);
		try {
			if (Difference_Union_Intersection == 1) {
				valid_union = CGAL::Polygon_mesh_processing::corefine_and_compute_union(lastMesh, meshList[i], output, CGAL::Polygon_mesh_processing::parameters::throw_on_self_intersection(true));//, throw_on_self_intersection
			} else if (Difference_Union_Intersection == 2) {
				valid_union = CGAL::Polygon_mesh_processing::corefine_and_compute_intersection(lastMesh, meshList[i], output, CGAL::Polygon_mesh_processing::parameters::throw_on_self_intersection(true));
			} else {
				valid_union = CGAL::Polygon_mesh_processing::corefine_and_compute_difference(lastMesh, meshList[i], output, CGAL::Polygon_mesh_processing::parameters::throw_on_self_intersection(true));
			}
			if (valid_union) {
				//myfile << "\n Valid Boolean";
				lastMesh = output;
			} else {
				//myfile << "\n Not Valid Boolean";
				lastMesh = meshList[i];
			}
		} catch (const std::exception& e) {
			lastMesh = meshList[i];
		}
		output.clear();
	}
	output = lastMesh;


	return true;
}



class command_mesh_difference : public CRhinoCommand {
public:

	command_mesh_difference() = default;
	~command_mesh_difference() = default;

	UUID CommandUUID() override {
		static const GUID joinery_solver_rhino7Command_UUID =
		{ 0x6f0776fb, 0x2e6b, 0x471f, { 0x9a, 0xd7, 0x38, 0x72, 0x3c, 0xc0, 0xfc, 0xa7 } };
		return joinery_solver_rhino7Command_UUID;
	}

	const wchar_t* EnglishCommandName() override { return L"joinery_solver_mesh_difference"; }
	CRhinoCommand::result RunCommand(const CRhinoCommandContext& context) override;
};

static class command_mesh_difference thejoinery_solver_rhino7Command;



CRhinoCommand::result command_mesh_difference::RunCommand(const CRhinoCommandContext& context) {

	///////////////////////////////////////////////////////////////////////////////////
	//Get Mesh
	///////////////////////////////////////////////////////////////////////////////////

	CRhinoGetObject go;
	go.SetCommandPrompt(L"Select 2 meshes");
	go.SetGeometryFilter(CRhinoGetObject::mesh_object);
	//o.GetObjects(2, 2);
	go.GetObjects(1, 0);

	if (go.CommandResult() != CRhinoCommand::success) return CRhinoCommand::failure;

	//Once pointcloud is selected get parameters
	std::vector<CGAL::Surface_mesh<CGAL::Exact_predicates_inexact_constructions_kernel::Point_3> > mesh_list;
	mesh_list.reserve(go.ObjectCount());

	for (int i = 0; i < go.ObjectCount(); i++) {
		const ON_Mesh* meshRhino = go.Object(i).Mesh();
		if (!(meshRhino && meshRhino->IsSolid())) continue;
		CGAL::Surface_mesh<CGAL::Exact_predicates_inexact_constructions_kernel::Point_3> mesh = Convert_RhinoMeshToCGAL(meshRhino);
		if (!mesh.is_valid(false)) continue;
		mesh_list.emplace_back(mesh);

	}


	RhinoApp().Print("============================================================================== CPP + \n");
	auto start = std::chrono::high_resolution_clock::now();

	////////////////////////////////////////////////////////////////
	// Perform CGAL Boolean
	////////////////////////////////////////////////////////////////
	CGAL::Surface_mesh<CGAL::Exact_predicates_inexact_constructions_kernel::Point_3> outCGAL;
	//MeshBoolean_CreateArrayTrackColors(mesh_list, outCGAL, 0);
	MeshBoolean_CreateArray(mesh_list, outCGAL, 0);
	//bool valid_union = CGAL::Polygon_mesh_processing::corefine_and_compute_difference(mesh1, mesh2, outCGAL);
	//bool valid_union = CGAL::Polygon_mesh_processing::corefine_and_compute_intersection(mesh1,mesh2, out);
	//bool valid_union = CGAL::Polygon_mesh_processing::corefine_and_compute_union(mesh1,mesh2, out);

	////////////////////////////////////////////////////////////////
	// Convert to Rhino Mesh
	////////////////////////////////////////////////////////////////
	ON_Mesh output = Convert_CGALMeshToRhino(outCGAL);


	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	RhinoApp().Print("==================================== %d ms ==================================== \n", duration);
	RhinoApp().Print("============================================================================== CPP - \n");

	/////////////////////////////////////////////////////////////////////
	//Output
	/////////////////////////////////////////////////////////////////////

	//Create layer
	const wchar_t* layer_name = L"joinery_solver_mesh";
	int layer_index = RhinoFindOrCreateLayer(context.m_doc, layer_name);
	if (layer_index == ON_UNSET_INT_INDEX)
		return CRhinoCommand::failure;

	ON_3dmObjectAttributes attributes;
	context.m_doc.GetDefaultObjectAttributes(attributes);
	attributes.m_layer_index = layer_index;


	////////////////////////////////////////////////////////////////
	// Add Rhino Mesh and delete input meshes
	////////////////////////////////////////////////////////////////

	if (output.IsValid()) {
		auto meshObjBaked = context.m_doc.AddMeshObject(output, &attributes);

		context.m_doc.DeleteObject(go.Object(0));
		context.m_doc.DeleteObject(go.Object(1));
		context.m_doc.Redraw();
	}

	context.m_doc.Redraw();



	return CRhinoCommand::success;
}

#pragma endregion

