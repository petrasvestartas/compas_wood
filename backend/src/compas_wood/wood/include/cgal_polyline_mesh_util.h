
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DEVELOPER:
// Petras Vestartas, petasvestartas@gmail.com
// Funding: NCCR Digital Fabrication and EPFL
//
// HISTORY:
// 1) The first version was written during the PhD 8928 thesis of Petras Vestartas called:
// Design-to-Fabrication Workflow for Raw-Sawn-Timber using Joinery Solver, 2017-2021
// 2) The translation from C# to C++ was started during the funding of NCCR in two steps
// A - standalone C++ version of the joinery solver and B - integration to COMPAS framework (Python Pybind11)
//
// RESTRICTIONS:
// The code cannot be used for commercial reasons
// If you would like to use or change the code for research or educational reasons,
// please contact the developer first
//
// 3RD PARTY LIBRARIES:
// CGAL CDT
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CGAL_POLYLINE_MESH_UTIL_H
#define CGAL_POLYLINE_MESH_UTIL_H

namespace cgal_polyline_mesh_util
{

    namespace internal
    {

                /**
         * Get length of a 3D vector, taken from the OpenNURBS library: https://github.com/mcneel/opennurbs
         *
         * @param [in] x The first coordinate of the vector
         * @param [in] y The second coordinate of the vector
         * @param [in] z The third coordinate of the vector
         * @return The length of the vector
         */
        double length(double x, double y, double z);

        /**
         * Unitize the vector by dividing its coordinates by its length
         *
         * @param [in, out] vector 3D vector
         * @return true if the vector length is not equal to zero
         */
        bool unitize(IK::Vector_3 &vector);

        /**
         * this functuon is used to mark facets that are inside the domain bounded by the polygon
         * it is used by mark_domains()
         *
         * @param [in] CGALCDT cgal constrained delaunay triangulation
         * @param [in] start face handle to start from
         * @param [in] index nesting level
         * @param [in, out] border border edges
         */
        void mark_domains(CGALCDT &ct, Face_handle start, int index, std::list<CGALCDT::Edge> &border);

        /**
         * explore set of facets connected with non constrained edges,
         * and attribute to each such set a nesting level.
         * We start from facets incident to the infinite vertex, with a nesting
         * level of 0. Then we recursively consider the non-explored facets incident
         * to constrained edges bounding the former set and increase the nesting level by 1.
         * Facets in the domain are those with an odd nesting level.
         *
         * @param [in] CGALCDT cgal constrained delaunay triangulation
         */

        void mark_domains(CGALCDT &CGALCDT);

        /**
         * Get transformation matrix from plane to XY plane
         *
         * @param [in] origin plane point
         * @param [in] plane CGAL plane
         * @return CGAL transformation matrix 4_4
         */
        CGAL::Aff_transformation_3<IK> plane_to_xy(const IK::Point_3 &origin, const IK::Plane_3 &plane);
    }

    /**
     * Create a mesh from a set of polylines with holes
     * WARNING: polylines_with_holes input contains top and bottom polylines, example top, bottom, top, bottom, top, bottom, ..., but only top polylines are used to create the mesh
     *
     * @param [in] polylines_with_holes polylines in the order: top, bottom, top, bottom, top, bottom, ...
     * @param [in] base_plane plane to orient to the first polyline
     * @param [out] top_outline_face_vertex_indices vector of vertex indices for top outline face
     * @param [out] v_count vertex count
     * @param [out] f_count face count
     */
    void mesh_from_polylines(const std::vector<CGAL_Polyline> &polylines_with_holes, const IK::Plane_3 &base_plane, std::vector<int> &top_outline_face_vertex_indices, int &v_count, int &f_count);

    /**
     * Create a mesh loft from a set of polylines with holes
     * "VNF" stands for "vertices, normals, faces
     *
     * @param [in] polylines_with_holes polylines in the order: top, bottom, top, bottom, top, bottom, ...
     * @param [out] out_vertices vertex coordinates: x,y,z,x,y,z,x,y,z,...
     * @param [out] out_normals vertex normals: x,y,z,x,y,z,x,y,z,...
     * @param [out] out_triangles triangle indices: i,j,k,i,j,k,i,j,k,...
     * @param [in] scale scale factor to convert scale the output geometry for viewer_wood
     * @return mesh vertices and faces in a form of Eigen matrices to be compatible with pybind11
     */
    void closed_mesh_from_polylines_vnf(const std::vector<CGAL_Polyline> &polylines_with_holes, std::vector<float> &out_vertices, std::vector<float> &out_normals, std::vector<int> &out_triangles, const double &scale = 1000);

}
#endif