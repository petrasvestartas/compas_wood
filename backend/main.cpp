#include "stdafx.h"
#include "wood_test.h" // test

int main(int argc, char **argv)
{

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// GoogleTest
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// wood_test::run_all_tests();

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Display
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	wood_globals::DISTANCE = 10;
	wood_globals::DISTANCE_SQUARED = 10;
	wood_globals::ANGLE = 1.11;

	opengl_globals::shader_type_0default_1transparent_2shaded_3shadedwireframe_4wireframe_5normals_6explode = 2;
	// opengl_globals::shaders_folder = "C:\\IBOIS57\\_Code\\Software\\Python\\compas_wood\\backend\\src\\viewer\\shaders\\";
	opengl_globals::shaders_folder = "C:\\legion_pro_7_16IRX8H\\software\\python\\compas_wood\\backend\\src\\viewer\\shaders\\";
	wood_globals::DATA_SET_INPUT_FOLDER = "C:\\legion_pro_7_16IRX8H\\software\\python\\compas_wood\\backend\\src\\wood\\dataset\\";
	opengl_render::render(wood_test::type_plates_name_vda_floor_1);

	// develop algorithm for notches
	

	// bool type_plates_name_cross_square_reciprocal_two_sides();
	// bool type_plates_name_cross_square_reciprocal_iseya();
	// bool type_plates_name_cross_ibois_pavilion();
	return 0;
}
