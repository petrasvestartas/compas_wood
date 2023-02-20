#include "stdafx.h"
#include "wood_test.h" // test

int main(int argc, char **argv)
{


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// GoogleTest
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	wood_test::run_all_tests();

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Display
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	opengl_globals::shader_type_0default_1transparent_2shaded_3shadedwireframe_4wireframe_5normals_6explode = 2;
	opengl_globals::shaders_folder = "C:\\IBOIS57\\_Code\\Software\\Python\\compas_wood\\backend\\src\\viewer\\shaders\\";
	opengl_render::render(wood_test::type_plates_name_joint_linking_vidychapel_one_layer);

	// Failing with the line merge:
	//  wood_xml->read_xml_polylines->C :\IBOIS57\_Code\Software\Python\compas_wood\backend\src\wood\dataset\type_plates_name_joint_linking_vidychapel_one_layer.xml
	// 		wood_xml->read_xml_polylines |
	// 	file exists
	// 		CPP Not simple
	// 			CPP Not simple
	// 				CPP Not simple

	// opengl_render::render(wood_test::type_plates_name_top_to_side_snap_fit);
	//	opengl_render::render(wood_test::type_library_name_ts_e_p_5);
	//      opengl_render::render(wood_test::type_library_name_ss_e_ip_2);
	return 0;
}
