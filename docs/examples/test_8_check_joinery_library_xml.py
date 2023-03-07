# compas_wood
from compas_wood.joinery import check_joinery_library_xml

# viewer
from compas_wood.viewer_helpers import disply_joint

(
    male_plines_polylines,
    female_plines_polylines,
    male_boolean_types,
    female_boolean_types,
) = check_joinery_library_xml(22, 150, 0.5)
# 1 2 9   10 11 12 19   20 21 22 23 29   30 31 39    49    59
# 1 - weird intersecting segment low distances issues - 22 23


disply_joint(
    male_plines_polylines,
    female_plines_polylines,
    male_boolean_types,
    female_boolean_types,
)
