
#include "../../../stdafx.h" //go up to the folder where the CMakeLists.txt is

#include "cgal_mesh_boolean.h"

namespace cgal_mesh_boolean
{

    // typedef boost::graph_traits<Mesh>::vertex_descriptor vertex_descriptor;
    //  typedef IK::Vector_3 Vector;
    //  typedef boost::property_map<Mesh, CGAL::vertex_point_t>::type VPMap;
    //  typedef Mesh::template Property_map<boost::graph_traits<Mesh>::vertex_descriptor, Vector> VNMap;

    struct Visitor : public PMP::Corefinement::Default_visitor<Mesh>
    {
        typedef Mesh::Face_index face_descriptor;

        boost::container::flat_map<const Mesh *, Mesh::Property_map<Mesh::Face_index, int>> properties;
        int face_id;

        Visitor()
        {
            properties.reserve(3);
            face_id = -1;
        }

        // visitor API overloaded
        void before_subface_creations(face_descriptor f_split, Mesh &tm)
        {
            face_id = properties[&tm][f_split];
        }

        void after_subface_created(face_descriptor f_new, Mesh &tm)
        {
            properties[&tm][f_new] = face_id;
        }

        void after_face_copy(face_descriptor f_src, Mesh &tm_src,
                             face_descriptor f_tgt, Mesh &tm_tgt)
        {
            properties[&tm_tgt][f_tgt] = properties[&tm_src][f_src];
        }
    };

    // https://github.com/CGAL/cgal/blob/master/Polygon_mesh_processing/examples/Polygon_mesh_processing/corefinement_mesh_union_with_attributes.cpp
    void mesh_boolean_create_array_track_colors(

        // double *coord_mesh, int *n_coord_meshArray, // Flat array of coordinates / 0 256 512 flat array of vertices array / number of meshes
        // int *faces_mesh, int *n_faces_meshArray,
        // size_t n_mesh,
        std::vector<Mesh> mesh_list,
        size_t difference_union_intersection,

        std::vector<float> &coord_out, int &n_coord_out,
        std::vector<float> &normals_out,
        std::vector<int> &faces_out, int &n_faces_out,
        std::vector<int> &facesColors_out, int &n_facesColors_out,
        int &n_valid_meshes

    )
    {

        if (n_valid_meshes == 9)
        {
            n_valid_meshes = 0;
            for (int i = 0; i < mesh_list.size(); i++)
            {
                if (mesh_list[i].is_valid(false))
                    n_valid_meshes++;
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Create Mesh
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // std::vector<Mesh::Property_map<Mesh::Face_index, int>> mesh_list_id;

        //     int numOfValidMeshes = 0;
        //     // int numOfMeshes = 0;
        //     // std::ofstream myfile;
        //     // std::ofstream myfile00;
        //     // myfile00.open ("C:\\libs\\PInvokeCPPCSHARP\\PInvoke\\x64\\Release\\TrackFaceIDStart00.txt");

        //     for (size_t i = 0; i < n_mesh; i++)
        //     {

        //         CGAL::Surface_mesh<CGAL::Exact_predicates_inexact_constructions_kernel::Point_3> tempMesh;
        //         // tempMesh.reserve(n_coord_meshArray[i+1]-n_coord_meshArray[i], (n_coord_meshArray[i+1]-n_coord_meshArray[i]) * 3, n_faces_meshArray[i+1]-n_faces_meshArray[i]);

        //         for (int j = n_coord_meshArray[i]; j < n_coord_meshArray[i + 1]; j++)
        //         {
        //             tempMesh.add_vertex(CGAL::Epick::Point_3(coord_mesh[3 * j + 0], coord_mesh[3 * j + 1], coord_mesh[3 * j + 2]));
        //         }

        //         for (int j = n_faces_meshArray[i]; j < n_faces_meshArray[i + 1]; j++)
        //         {
        //             tempMesh.add_face(CGAL::SM_Vertex_index(faces_mesh[3 * j + 0]), CGAL::SM_Vertex_index(faces_mesh[3 * j + 1]), CGAL::SM_Vertex_index(faces_mesh[3 * j + 2]));
        //         }

        //         if (tempMesh.is_valid(false))
        //         {

        //             /////////////////////////////////////////////////////////////////
        //             // Add Mesh
        //             /////////////////////////////////////////////////////////////////
        //             mesh_list.push_back(tempMesh);
        //             numOfValidMeshes++;
        //         }
        //         // myfile << "\n ";
        //         // numOfMeshes++;
        //     }

        //////////////////////////////////////////////////////////////////
        //// Perform CGAL Boolean
        //////////////////////////////////////////////////////////////////

        CGAL::Surface_mesh<CGAL::Exact_predicates_inexact_constructions_kernel::Point_3> &lastMesh = mesh_list[0];
        Mesh::Property_map<Mesh::Face_index, int> lastMesh_id = lastMesh.add_property_map<Mesh::Face_index, int>("f:id", -1).first;
        for (Mesh::Face_index f : faces(lastMesh))
            lastMesh_id[f] = 1;

        for (int i = 1; i < mesh_list.size(); i++)
        { // mesh_list.size ()

            Visitor visitor;

            // Properties last mesh
            // Mesh::Property_map<Mesh::Face_index, int>  lastMesh_id = lastMesh.add_property_map<Mesh::Face_index, int> ("f:id", -1).first;
            // for ( Mesh::Face_index f : faces (lastMesh) )
            //     lastMesh_id[f] = 1;
            visitor.properties[&lastMesh] = lastMesh_id; // From previous iteration must or must not contain property map?

            ////Properties current mesh
            Mesh::Property_map<Mesh::Face_index, int> mesh_id = mesh_list[i].add_property_map<Mesh::Face_index, int>("f:id", -1).first;
            for (Mesh::Face_index f : faces(mesh_list[i]))
                mesh_id[f] = (i + 1);
            visitor.properties[&mesh_list[i]] = mesh_id;

            ////Properties Out
            Mesh out;
            Mesh::Property_map<Mesh::Face_index, int> out_id = out.add_property_map<Mesh::Face_index, int>("f:id", -1).first;
            visitor.properties[&out] = out_id;

            bool valid_union = false;
            const bool throw_on_self_intersection = true;

            try
            {

                if (difference_union_intersection == 1)
                {
                    valid_union = CGAL::Polygon_mesh_processing::corefine_and_compute_union(lastMesh, mesh_list[i], out, CGAL::Polygon_mesh_processing::parameters::visitor(visitor), CGAL::Polygon_mesh_processing::parameters::throw_on_self_intersection(true)); //, , CGAL::Polygon_mesh_processing::parameters::throw_on_self_intersection (true) , CGAL::Polygon_mesh_processing::parameters::visitor (visitor)
                }
                else if (difference_union_intersection == 2)
                {
                    valid_union = CGAL::Polygon_mesh_processing::corefine_and_compute_intersection(lastMesh, mesh_list[i], out, CGAL::Polygon_mesh_processing::parameters::visitor(visitor), CGAL::Polygon_mesh_processing::parameters::throw_on_self_intersection(true)); //, CGAL::Polygon_mesh_processing::parameters::visitor (visitor)
                }
                else
                {
                    valid_union = CGAL::Polygon_mesh_processing::corefine_and_compute_difference(lastMesh, mesh_list[i], out, CGAL::Polygon_mesh_processing::parameters::visitor(visitor), CGAL::Polygon_mesh_processing::parameters::throw_on_self_intersection(true)); //, CGAL::Polygon_mesh_processing::parameters::visitor (visitor)
                }

                // std::ofstream myfile3;
                // myfile3.open ("C:\\libs\\PInvokeCPPCSHARP\\PInvoke\\x64\\Release\\TrackFaceIDEnd.txt");
                // for ( Mesh::Face_index f : faces (out) )
                //     myfile3 << f << " ID" << out_id[f] << " \n";
                // myfile3.close ();

                // std::cout << "Is Mesh Boolean Valid: " << valid_union << "\n";
                lastMesh = valid_union ? out : mesh_list[i];

                // for ( Mesh::Face_index f : faces (out) )
                //     lastMesh_id[f] = out_id[f];

                lastMesh_id = lastMesh.add_property_map<Mesh::Face_index, int>("f:id", -1).first;
                for (Mesh::Face_index f : faces(out))
                {
                    auto faceID = out_id[f];
                    lastMesh_id[f] = faceID;
                }
            }
            catch (const std::exception &e)
            {
                lastMesh = mesh_list[i];
            }
        }

        ////////////////////////////////////////////////////////////////
        // Return vertex and face array
        ////////////////////////////////////////////////////////////////

        // Compute mesh vertex normals
        Mesh::Property_map<boost::graph_traits<Mesh>::vertex_descriptor, IK::Vector_3> vnormals = lastMesh.template add_property_map<boost::graph_traits<Mesh>::vertex_descriptor, IK::Vector_3>("v:normals", CGAL::NULL_VECTOR).first;
        CGAL::Polygon_mesh_processing::compute_vertex_normals(lastMesh, vnormals);

        const int vc = lastMesh.number_of_vertices() * 3;
        coord_out = std::vector<float>(vc);
        normals_out = std::vector<float>(vc);

        int i = 0;
        int c = 0;
        for (auto &ni : vnormals)
        {
            normals_out[i + 0] = (float)ni.hx();
            normals_out[i + 1] = (float)ni.hy();
            normals_out[i + 2] = (float)ni.hz();
            // std::cout << normals_out[i + 0] << " " << normals_out[i + 1] << " " << normals_out[i + 2] << " \n";
            i += 3;
            c++;
        }
        // Output
        //  Get vertices coordinates

        i = 0;
        c = 0;
        for (const auto &vi : lastMesh.vertices())
        {
            const auto &pt = lastMesh.point(vi);
            // std::cout << (float)pt.x() << " " << (float)pt.y() << " " << (float)pt.z() << " \n";
            coord_out[i + 0] = (float)pt.x();
            coord_out[i + 1] = (float)pt.y();
            coord_out[i + 2] = (float)pt.z();
            i += 3;
            c++;
        }

        n_coord_out = c;

        // Get face indices
        const int fc = lastMesh.number_of_faces() * 3;
        faces_out = std::vector<int>(fc);

        i = 0;
        c = 0;
        for (auto face_index : lastMesh.faces())
        {
            std::vector<uint32_t> indices;
            CGAL::Vertex_around_face_circulator<CGAL::Surface_mesh<CGAL::Exact_predicates_inexact_constructions_kernel::Point_3>> vcirc(lastMesh.halfedge(face_index), lastMesh), done(vcirc);
            do
                indices.push_back(*vcirc++);
            while (vcirc != done);
            // meshRebuilt.SetTriangle (face_index.idx (), indices[0], indices[1], indices[2]);

            faces_out[i++] = (int)indices[0];
            faces_out[i++] = (int)indices[1];
            faces_out[i++] = (int)indices[2];
            // facesColors_out[c] = (int)lastMesh_id[face_index];
            c++;
        }

        n_faces_out = c;

        // Get face color indices
        const int fcID = lastMesh.number_of_faces();
        facesColors_out = std::vector<int>(fcID);

        // std::ofstream myfile3;
        // myfile3.open ("C:\\libs\\PInvokeCPPCSHARP\\PInvoke\\x64\\Release\\TrackFaceIDEnd.txt");
        // myfile3 << fcID;

        int IDCounter = 0;
        for (Mesh::Face_index f : faces(lastMesh))
        {
            int id = lastMesh_id[f];
            // myfile3 << "\n" << f << " ID" << lastMesh_id[f] << " " << IDCounter ;
            facesColors_out[IDCounter] = id;
            IDCounter++;
        }

        n_facesColors_out = IDCounter;
        // myfile3.close ();
    }

    void mesh_boolean_test()
    {
        // data-set taken from https://schneide.blog/tag/cc/
        CGAL::Surface_mesh<CGAL::Exact_predicates_inexact_constructions_kernel::Point_3> mesh_0;
        CGAL::Surface_mesh<CGAL::Exact_predicates_inexact_constructions_kernel::Point_3> mesh_1;

        mesh_0.add_vertex(CGAL::Epick::Point_3(0, 0, 0));
        mesh_0.add_vertex(CGAL::Epick::Point_3(0, 0, 1));
        mesh_0.add_vertex(CGAL::Epick::Point_3(1, 0, 1));
        mesh_0.add_vertex(CGAL::Epick::Point_3(1, 1, 1));
        mesh_0.add_vertex(CGAL::Epick::Point_3(1, 0, 0));
        mesh_0.add_vertex(CGAL::Epick::Point_3(1, 1, 0));
        mesh_0.add_vertex(CGAL::Epick::Point_3(0, 1, 0));
        mesh_0.add_vertex(CGAL::Epick::Point_3(0, 1, 1));

        mesh_0.add_face(CGAL::SM_Vertex_index(0), CGAL::SM_Vertex_index(4), CGAL::SM_Vertex_index(2));
        mesh_0.add_face(CGAL::SM_Vertex_index(4), CGAL::SM_Vertex_index(5), CGAL::SM_Vertex_index(3));
        mesh_0.add_face(CGAL::SM_Vertex_index(5), CGAL::SM_Vertex_index(6), CGAL::SM_Vertex_index(7));
        mesh_0.add_face(CGAL::SM_Vertex_index(6), CGAL::SM_Vertex_index(0), CGAL::SM_Vertex_index(1));
        mesh_0.add_face(CGAL::SM_Vertex_index(4), CGAL::SM_Vertex_index(0), CGAL::SM_Vertex_index(6));
        mesh_0.add_face(CGAL::SM_Vertex_index(6), CGAL::SM_Vertex_index(5), CGAL::SM_Vertex_index(4));
        mesh_0.add_face(CGAL::SM_Vertex_index(2), CGAL::SM_Vertex_index(7), CGAL::SM_Vertex_index(1));
        mesh_0.add_face(CGAL::SM_Vertex_index(7), CGAL::SM_Vertex_index(2), CGAL::SM_Vertex_index(3));
        mesh_0.add_face(CGAL::SM_Vertex_index(0), CGAL::SM_Vertex_index(2), CGAL::SM_Vertex_index(1));
        mesh_0.add_face(CGAL::SM_Vertex_index(4), CGAL::SM_Vertex_index(3), CGAL::SM_Vertex_index(2));
        mesh_0.add_face(CGAL::SM_Vertex_index(5), CGAL::SM_Vertex_index(7), CGAL::SM_Vertex_index(3));
        mesh_0.add_face(CGAL::SM_Vertex_index(6), CGAL::SM_Vertex_index(1), CGAL::SM_Vertex_index(7));

        mesh_1.add_vertex(CGAL::Epick::Point_3(0.353451, 0.209961, 0.725612));
        mesh_1.add_vertex(CGAL::Epick::Point_3(0.353451, 0.209961, 1.725612));
        mesh_1.add_vertex(CGAL::Epick::Point_3(1.353451, 0.209961, 1.725612));
        mesh_1.add_vertex(CGAL::Epick::Point_3(1.353451, 1.209961, 1.725612));
        mesh_1.add_vertex(CGAL::Epick::Point_3(1.353451, 0.209961, 0.725612));
        mesh_1.add_vertex(CGAL::Epick::Point_3(1.353451, 1.209961, 0.725612));
        mesh_1.add_vertex(CGAL::Epick::Point_3(0.353451, 1.209961, 0.725612));
        mesh_1.add_vertex(CGAL::Epick::Point_3(0.353451, 1.209961, 1.725612));

        mesh_1.add_face(CGAL::SM_Vertex_index(0), CGAL::SM_Vertex_index(4), CGAL::SM_Vertex_index(2));
        mesh_1.add_face(CGAL::SM_Vertex_index(4), CGAL::SM_Vertex_index(5), CGAL::SM_Vertex_index(3));
        mesh_1.add_face(CGAL::SM_Vertex_index(5), CGAL::SM_Vertex_index(6), CGAL::SM_Vertex_index(7));
        mesh_1.add_face(CGAL::SM_Vertex_index(6), CGAL::SM_Vertex_index(0), CGAL::SM_Vertex_index(1));
        mesh_1.add_face(CGAL::SM_Vertex_index(4), CGAL::SM_Vertex_index(0), CGAL::SM_Vertex_index(6));
        mesh_1.add_face(CGAL::SM_Vertex_index(6), CGAL::SM_Vertex_index(5), CGAL::SM_Vertex_index(4));
        mesh_1.add_face(CGAL::SM_Vertex_index(2), CGAL::SM_Vertex_index(7), CGAL::SM_Vertex_index(1));
        mesh_1.add_face(CGAL::SM_Vertex_index(7), CGAL::SM_Vertex_index(2), CGAL::SM_Vertex_index(3));
        mesh_1.add_face(CGAL::SM_Vertex_index(0), CGAL::SM_Vertex_index(2), CGAL::SM_Vertex_index(1));
        mesh_1.add_face(CGAL::SM_Vertex_index(4), CGAL::SM_Vertex_index(3), CGAL::SM_Vertex_index(2));
        mesh_1.add_face(CGAL::SM_Vertex_index(5), CGAL::SM_Vertex_index(7), CGAL::SM_Vertex_index(3));
        mesh_1.add_face(CGAL::SM_Vertex_index(6), CGAL::SM_Vertex_index(1), CGAL::SM_Vertex_index(7));

        std::vector<Mesh> mesh_list;
        mesh_list.reserve(2);
        mesh_list.emplace_back(mesh_0);
        mesh_list.emplace_back(mesh_1);

        // run the boolean intersection

        size_t difference_union_intersection = 0;

        std::vector<float> coord_out;
        std::vector<float> normals_out;
        int n_coord_out;
        std::vector<int> faces_out;
        int n_faces_out;
        std::vector<int> facesColors_out;
        int n_facesColors_out;
        int n_valid_meshes = 9;

        cgal_mesh_boolean::mesh_boolean_create_array_track_colors(

            // double *coord_mesh, int *n_coord_meshArray, // Flat array of coordinates / 0 256 512 flat array of vertices array / number of meshes
            // int *faces_mesh, int *n_faces_meshArray,
            // size_t n_mesh,
            mesh_list,
            difference_union_intersection,

            coord_out, n_coord_out,
            normals_out,
            faces_out,
            n_faces_out,
            facesColors_out, n_facesColors_out,
            n_valid_meshes

        );

        std::cout << "\n__________________________________________________________\n mesh_boolean_test() \n n_coord_out is 22 ? " << n_coord_out << "\n n_normals_out is 22 ? " << normals_out.size() / 3 << "\n n_faces_out is 40 ? " << n_faces_out << "\n n_facesColors_out is 40 ? " << n_facesColors_out << "\n n_valid_meshes is 2 ? " << n_valid_meshes << "\n__________________________________________________________\n";
    }
}