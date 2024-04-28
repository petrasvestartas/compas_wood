from compas_wood.binding import wood_globals

# Property used in get_connection_zones function.
# Pacakge compas_wood relies on extensive joinery library.
# This parameter helps to choose a specific joint parameters.
# 300, 0.5, 3,   // 1-9 ss_e_ip (side-to-side edge in-plane)
# 450, 0.64, 15, // 10-19 ss_e_op (side-to-side edge out-of-plane)
# 450, 0.5, 20,  // 20-29 ts_e_p (top-to-side edge plane)
# 300, 0.5, 30,  // 30-39 cr_c_ip (cross cutting in-plane)
# 6, 0.95, 40,   // 40-49 tt_e_p  (top-to-top edge plane)
# 300, 0.5, 58,  // 50-59 ss_e_r (side-to-side edge rotated)        
# 300, 1.0, 60   // 60-69 b (boundary)
print(wood_globals.joints_parameters_and_types)
