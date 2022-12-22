#include "stdafx.h"
// temp
// #include "wood_main.h"
// #include "wood_xml.h"	 // xml parser and viewer customization
// #include "viewer_wood.h" // viewer
#include "wood_test.h" // test

// #include <polylabel/polylabel.h>
#include <polylabel/cgal_polylabel.h>
#include "src/wood/include/cgal_rectangle_util.h"
int main(int argc, char **argv)
{

	 wood_test::run_all_tests();

	// IK::Segment_3 segment(IK::Point_3(0, 0, 0), IK::Point_3(1, 1, 0));
	// // std::cout << cgal_rectangle_util::angle_to_xaxis(segment) << "\n";

	CGAL_Polyline polygon{
		IK::Point_3(11.2247596176396, -4.60190643733344, 48.9387618537435),
		IK::Point_3(-5.73799008527948, 27.7396887390699, 38.9994193935361),
		IK::Point_3(-7.18751465466158, 52.2109764075134, 74.5663582911656),
		IK::Point_3(1.68332949862521, 18.8956298287167, 52.2485552589767),
		IK::Point_3(14.0704361211272, 1.10346344393741, 69.2793807577245),
		IK::Point_3(18.9418950031543, 14.2294672539151, 109.735250000281),
		IK::Point_3(33.4352381327694, -30.7000564810127, 89.2118909021016),
		IK::Point_3(11.2247596176396, -4.60190643733344, 48.9387618537435),
	};

	// CGAL_Polyline rectangle;
	// cgal_rectangle_util::bounding_rectangle(polygon, rectangle);
	// for (auto &point : rectangle)
	// 	std::cout << point.hx() << " " << point.hy() << " " << point.hz() << "\n";
	std::vector<IK::Point_3> points;
	cgal_rectangle_util::grid_of_points_in_a_polygon(polygon, 0, points);
	// std::vector<CGAL_Polyline>
	// 	polylines;

	// CGAL_Polyline p0{
	// 	IK::Point_3(-51.268164698738, 124.403259575835, 0),
	// 	IK::Point_3(-49.9053069162896, 125.207270883064, 0),
	// 	IK::Point_3(-49.2387999055025, 123.598641360433, 0),
	// 	IK::Point_3(-51.268164698738, 124.403259575835, 0),
	// };

	// CGAL_Polyline p1{
	// 	IK::Point_3(-80.6608706869908, 113.550451407233, 0),
	// 	IK::Point_3(-105.295437496349, 124.537127782527, 0),
	// 	IK::Point_3(-76.4480884206953, 152.30682138802, 0),
	// 	IK::Point_3(-23.6934094178782, 139.390066049447, 0),
	// 	IK::Point_3(-40.1282334662578, 100.282298806669, 0),
	// 	IK::Point_3(-69.6350088663798, 87.7972305312764, 0),
	// 	IK::Point_3(-74.8538706538141, 108.464053813429, 0),
	// 	IK::Point_3(-80.6608706869908, 113.550451407233, 0),
	// };

	// CGAL_Polyline p2{
	// 	IK::Point_3(-73.4808769486925, 119.997690099402, 0),
	// 	IK::Point_3(-90.6424155869293, 123.188472670033, 0),
	// 	IK::Point_3(-77.6978215210285, 136.742511096638, 0),
	// 	IK::Point_3(-73.4808769486925, 119.997690099402, 0),
	// };

	// polylines.push_back(p0);
	// polylines.push_back(p1);
	// polylines.push_back(p2);

	// std::tuple<IK::Point_3, IK::Plane_3, double> result = cgal_polylabel::get_polylabel(polylines, 1.0);
	// std::cout << std::get<0>(result).hx() << " " << std::get<0>(result).hy() << " " << std::get<0>(result).hz() << std::endl;
	// std::cout << std::get<1>(result).orthogonal_vector().hx() << " " << std::get<1>(result).orthogonal_vector().hy() << " " << std::get<1>(result).orthogonal_vector().hz() << std::endl;
	// std::cout << std::get<2>(result) << std::endl;
	// 	mapbox::geometry::polygon<double>
	// 		polygon = {
	// 			{{25.003161, 16.395586}, {0.368594, 27.382262}, {29.215943, 55.151956}, {81.970622, 42.2352}, {65.535798, 3.127433}, {36.029023, -9.357635}, {30.810161, 11.309188}},
	// 			{{32.183155, 22.842825}, {15.021616, 26.033607}, {27.96621, 39.587646}}}; // Get polygon data from somewhere.
	// std::array<double, 3> center_and_radius = mapbox::polylabel(polygon, 1.0);
	// std::cout << center_and_radius[0] << "," << center_and_radius[1] << "," << center_and_radius[2] << std::endl;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Display
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	opengl_globals::shader_type_0default_1transparent_2shaded_3shadedwireframe_4wireframe_5normals_6explode = 2;
	opengl_globals::shaders_folder = "C:\\IBOIS57\\_Code\\Software\\Python\\compas_wood\\backend\\src\\viewer\\shaders\\";
	opengl_render::render(wood_test::type_plates_name_joint_linking_vidychapel_corner); // chapel_corner// chapel_one_layer // chapel_simple // chapel

	return 0;
}

// run ninja-> g++ main.cpp -o main
// cmake  --fresh -DGET_LIBS=OFF -DBUILD_MY_PROJECTS=ON -DRELEASE_DEBUG=ON -DCMAKE_BUILD_TYPE="Release" -G "Ninja" .. && cmake  --build . --config Release
// cmake --fresh -DGET_LIBS=OFF -DBUILD_MY_PROJECTS=ON -DRELEASE_DEBUG=ON -DCMAKE_BUILD_TYPE="Release"  -G "Visual Studio 17 2022" -A x64 .. && cmake --build . --config Release
// cmake  -E time --build . -v --config Release -- -j0
// cmake  --build . -v --config Release -- -j0
// cmake  --build . --config Release -v