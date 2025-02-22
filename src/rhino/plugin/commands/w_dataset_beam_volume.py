# flags: python.reloadEngine
#! python3
# venv: timber_connections
from wood_nano import beam_volumes as wood_nano_beam_volumes
import wood_nano as m

from wood_nano.conversions_python import to_double2
from wood_nano.conversions_python import to_int1
from wood_nano import int1
from wood_nano import int2
from wood_nano import double2
from wood_nano import point2
from wood_nano import point3
from wood_nano import cut_type2
import Rhino
from Rhino.Geometry import Polyline, Point3d, Line, Vector3d, RTree, RTreeEventArgs, Sphere
from typing import *
from wood_rui import add_polylines, wood_rui_globals, generalized_input_method, add_axes


def closest_lines_to_polylines(dataset_name: str, lines: List[Line]) -> None:
    """
    Match the closest lines from the selected dataset to the input lines and update
    the insertion vectors.

    Parameters
    ----------
    dataset_name : str
        The name of the dataset to use.
    lines : List[Line]
        List of lines to compare against the polylines in the dataset.
    """

    # Convert polylines to pairs
    polylines: List[List[Any]] = []  # This will hold the list of lists

    # Flat list of polylines
    flat_polylines: List[Any] = wood_rui_globals[dataset_name]["axes"]

    # Convert element polylines to a flattened dictionary
    segments_dictionary: Dict[int, List[Any]] = {}
    vectors: List[List[Vector3d]] = []
    count: int = 0

    # Populate segments dictionary and initialize vectors
    for i in range(len(polylines)):
        vectors.append([Vector3d.ZAxis()] * (polylines[i][0].SegmentCount))
        for j in range(polylines[i].SegmentCount):
            bbox = polylines[i].SegmentAt(j).BoundingBox
            bbox.Inflate(0.02)
            segments_dictionary[count] = [i, j, bbox, polylines[i][j].SegmentAt(j)]
            count += 1

    # Create and fill the RTree
    rtree = RTree()
    for i in segments_dictionary:
        rtree.Insert(segments_dictionary[i][3], i)

    # Callbacks for RTree search
    def search_callback(sender: Any, rtree_event_args: RTreeEventArgs) -> None:
        data_by_reference.append(rtree_event_args.Id)

    # Perform search for both start and end points of each line
    for i in range(len(lines)):
        data_by_reference: List[int] = []
        if rtree.Search(Sphere(lines[i].From, 0), search_callback, data_by_reference):
            for j in data_by_reference:
                if lines[i].From.DistanceToSquared(segments_dictionary[j][4].ClosestPoint(lines[i].From, True)) < 0.001:
                    vectors[segments_dictionary[j][0]][segments_dictionary[j][1]] = lines[i].Direction

    for i in range(len(lines)):
        data_by_reference: List[int] = []
        if rtree.Search(Sphere(lines[i].To, 0), search_callback, data_by_reference):
            for j in data_by_reference:
                if lines[i].To.DistanceToSquared(segments_dictionary[j][4].ClosestPoint(lines[i].To, True)) < 0.001:
                    vectors[segments_dictionary[j][0]][segments_dictionary[j][1]] = -lines[i].Direction

    # Store the insertion vectors in the global data structure
    wood_rui_globals[dataset_name]["insertion_vectors"] = vectors
    print("Number of plates with set insertion: ", len(wood_rui_globals[dataset_name]["insertion_vectors"]))

    # Update Rhino object attributes
    polylines_guid_list = wood_rui_globals[dataset_name]["axes_guid"]
    for i in range(0, len(polylines_guid_list)):
        guid = polylines_guid_list[i]
        rhino_object = Rhino.RhinoDoc.ActiveDoc.Objects.Find(guid)

        for idx, v in enumerate(vectors[i]):
            name = f"insertion_vector_{idx}"
            value = f"{v.X} {v.Y} {v.Z}"
            rhino_object.Attributes.SetUserString(name, value)


def default_dataset():
    return [
        Polyline(
            [
                Point3d(297.037012066934, 126.422271499885, -178.188030913779),
                Point3d(321.489409568039, 169.111942318764, -186.869036108803),
                Point3d(345.941807069144, 211.801613137642, -195.550041303827),
                Point3d(370.394204570249, 254.491283956521, -204.231046498851),
            ]
        ),
        Polyline(
            [
                Point3d[182.634858082301, 98.7137182074149, -78.306080246581],
                Point3d[211.235396578459, 105.640856530532, -103.27656791338],
                Point3d[239.835935074617, 112.56799485365, -128.24705558018],
                Point3d[268.436473570776, 119.495133176767, -153.217543246979],
                Point3d[297.037012066934, 126.422271499885, -178.188030913779],
            ]
        ),
        Polyline(
            [
                Point3d(-125.19241232501, 107.875422065027, -39.2649958323028),
                Point3d(-75.705277009549, 107.25372412331, -42.5192301219386),
                Point3d(-26.8203151230943, 105.243836458838, -50.6895979092208),
                Point3d(21.1131439030443, 102.043760643015, -63.0656900444498),
                Point3d(67.9151143837992, 97.8400491497485, -78.9813305497922),
                Point3d(113.521085871532, 92.7952640964856, -97.8579657684805),
                Point3d(157.936920791687, 87.0452505103035, -119.213391744547),
                Point3d(201.207350715535, 80.7012045875338, -142.653744283013),
            ]
        ),
        Polyline(
            [
                Point3d(-10.3599318150342, 38.7282123158674, -287.206518566188),
                Point3d(-31.7764187502288, 49.1160173176098, -249.870793280387),
                Point3d(-46.147480668278, 60.3975803199585, -209.510751764339),
                Point3d(-54.662795188934, 72.1419219251921, -167.63145233557),
                Point3d(-58.5361339307107, 84.0926783823342, -125.117457662038),
                Point3d(-58.7606503977529, 96.1110401814458, -82.4406121991002),
                Point3d(-56.0905042124202, 108.124674519414, -39.8417855305585),
                Point3d(-51.0879024066377, 120.096820292103, 2.56039219002248),
                Point3d(-44.1738209609492, 132.009588674272, 44.7113106559208),
            ]
        ),
        Polyline(
            [
                Point3d(-188.240159298455, 75.8808592794907, -151.530383786316),
                Point3d(-145.963074583616, 73.7325840014969, -160.05232439351),
                Point3d(-103.685989868776, 71.5843087235031, -168.574265000704),
                Point3d(-61.4089051539373, 69.4360334455093, -177.096205607898),
                Point3d(-19.1318204390981, 67.2877581675155, -185.618146215092),
                Point3d(23.1452642757411, 65.1394828895218, -194.140086822286),
            ]
        ),
        Polyline(
            [
                Point3d(-96.6199846476256, 90.1021988025274, -110.227043500606),
                Point3d(-48.9174102895825, 87.3983183807673, -111.648442107134),
                Point3d(-1.21483593153943, 84.6944379590072, -113.069840713663),
                Point3d(46.4877384265037, 81.9905575372471, -114.491239320191),
                Point3d(94.1903127845467, 79.286677115487, -115.91263792672),
            ]
        ),
        Polyline(
            [
                Point3d(132.972124919423, 138.449055887252, 86.074977722597),
                Point3d(167.267793637183, 134.152861891673, 62.679797641331),
                Point3d(201.563462354943, 129.856667896093, 39.2846175600651),
                Point3d(235.859131072703, 125.560473900514, 15.8894374787991),
            ]
        ),
        Polyline(
            [
                Point3d(129.410585091899, 71.8669625143157, -172.501597971961),
                Point3d(151.870897769568, 83.1961266976715, -132.75167444875),
                Point3d(174.331210447238, 94.5252908810273, -93.0017509255386),
                Point3d(196.791523124907, 105.854455064383, -53.2518274023273),
                Point3d(219.251835802577, 117.183619247739, -13.5019038791161),
                Point3d(241.712148480247, 128.512783431095, 26.2480196440951),
                Point3d(264.172461157916, 139.84194761445, 65.9979431673064),
            ]
        ),
        Polyline(
            [
                Point3d(164.938767042065, 160.473718227021, 163.598974047964),
                Point3d(147.837376054901, 149.074854470926, 123.488214523064),
                Point3d(131.218069322568, 137.625923389656, 83.1894892076615),
                Point3d(115.06311204992, 126.130413515365, 42.715559558341),
                Point3d(99.3556294473208, 114.591526704951, 2.07815097714067),
                Point3d(84.0795669982659, 103.012205524298, -38.7119491181474),
                Point3d(69.2196466793098, 91.3951546375981, -79.6448144234582),
                Point3d(54.7613339074588, 79.7428674496858, -120.71128445365),
                Point3d(40.6907906778078, 68.0576389813056, -161.902917836579),
                Point3d(26.9948513423117, 56.3415940716198, -203.2118916656),
            ]
        ),
        Polyline(
            [
                Point3d(114.821035246594, 198.058111884719, 106.829580839264),
                Point3d(139.879901144329, 179.26591505587, 135.214277443614),
                Point3d(164.938767042065, 160.473718227021, 163.598974047964),
                Point3d(189.9976329398, 141.681521398171, 191.983670652314),
                Point3d(215.056498837535, 122.889324569322, 220.368367256664),
            ]
        ),
        Polyline(
            [
                Point3d(264.172461157916, 139.84194761445, 65.9979431673064),
                Point3d(312.86620413861, 146.859577749078, 57.3169379722823),
                Point3d(361.559947119303, 153.877207883706, 48.6359327772582),
                Point3d(410.253690099997, 160.894838018334, 39.954927582234),
            ]
        ),
        Polyline(
            [
                Point3d(13.7562673016774, 164.804159616155, 178.132387775678),
                Point3d(31.0555420852146, 160.035948344655, 140.217407062733),
                Point3d(42.5288946455757, 155.666866376768, 100.123423196389),
                Point3d(46.7750711952287, 151.895205966801, 58.5935066719183),
                Point3d(42.9566247038049, 148.911475312983, 16.9636577178732),
                Point3d(31.3140030201931, 146.81417226681, -23.2519859176756),
            ]
        ),
    ]


def my_callback(
    name_value_type,
    dataset_name,
    input_flip_male=0,
    input_compute_joints=False,
    input_division_distance=150,
    input_shift=0.5,
    input_output_geometry_type=0,
    input_use_eccentricities_to_scale_joints=True,
):

    input_polyline_axes = name_value_type["polyline_axes"][0]
    input_radii = name_value_type["polylines_segment_radii"][0]
    input_polylines_segment_direction = name_value_type["polylines_segment_direction"][0]


    input_allowed_types_per_polyline = name_value_type["allowed_types_per_polyline"][0]
    input_min_distance = name_value_type["min_distance"][0]
    input_volume_length = name_value_type["volume_length"][0]
    input_cross_or_side_to_end = name_value_type["cross_or_side_to_end"][0]
    # input_flip_male = name_value_type["flip_male"]
    # input_compute_joints = name_value_type["compute_joints"]
    # input_division_distance = name_value_type["division_distance"]
    # input_shift = name_value_type["shift"]
    # input_output_geometry_type = name_value_type["output_geometry_type"]
    # input_use_eccentricities_to_scale_joints = name_value_type["use_eccentricities_to_scale_joints"]

    # Radius of each segment of each polyline.
    input_polylines_segment_radii = []

    for i in range(len(input_polyline_axes)):
        input_polyline_segment_radii = []
        for j in range(input_polyline_axes[i].SegmentCount):
            input_polyline_segment_radii.append(input_radii)
        input_polylines_segment_radii.append(input_polyline_segment_radii)

    # Direction of each segment of each polyline.
    input_polylines_segment_direction = []
    for i in range(len(input_polyline_axes)):
        input_polyline_segment_direction = []
        for j in range(input_polyline_axes[i].SegmentCount):
            input_polyline_segment_direction.append(Vector3d(0, 1, 0))
        input_polylines_segment_direction.append(input_polyline_segment_direction)
    

    print("___________________")
    print(input_polyline_axes)
    print(input_polylines_segment_radii)
    print(input_polylines_segment_direction)
    print("___________________")

    index_polylines = int2()
    index_polylines_segment = int2()
    distance = double2()
    point_pairs = point2()
    volume_pairs = point3()
    joints_areas = point2()
    joints_types = int1()
    w_output_plines = point3()
    w_output_types = cut_type2()

    def to_point(vectors, depth):
        if depth == 1:  # Single polyline
            polyline = m.point1()
            for point3d in vectors:
                polyline.append(m.point(point3d.X, point3d.Y, point3d.Z))
            return polyline
        else:  # Nested polyline
            polyline = getattr(m, f"point{depth}")()
            for vec in vectors:
                polyline.append(to_point(vec, depth - 1))
            return polyline

    def to_point2(polylines):
        return to_point(polylines, 2)

    def to_vector(vectors, depth):
        if depth == 1:  # Single vector_of_vectors
            vector_of_vectors = m.vector1()
            for vector3d in vectors:
                print(vector3d, type(vector3d))
                vector_of_vectors.append(m.vector(vector3d.X, vector3d.Y, vector3d.Z))
            return vector_of_vectors
        else:  # Nested vector_of_vectors
            vector = getattr(m, f"vector{depth}")()
            for vec in vectors:
                vector.append(to_vector(vec, depth - 1))
            return vector

    def to_vector2(vectors):
        return to_vector(vectors, 2)

    if len(input_polyline_axes) == 0:
        return

    if len(input_polylines_segment_radii) == 0:
        return

    if len(input_polylines_segment_direction) == 0:
        return

    print(input_polyline_axes)
    # input_polylines_segment_radii.clear()
    input_polylines_segment_direction.clear()
    print(input_polylines_segment_radii)
    print(input_polylines_segment_direction)
    print(input_allowed_types_per_polyline)
    print(input_min_distance)
    print(input_volume_length)
    print(input_cross_or_side_to_end)

    # a = to_point2(input_polyline_axes)
    # print(a[0][0][0], a[0][0][1], a[0][0][2])
    # print(a[0][1][0], a[0][1][1], a[0][1][2])

    # print(a[1][0][0], a[1][0][1], a[1][0][2])
    # print(a[1][1][0], a[1][1][1], a[1][1][2])

    wood_nano_beam_volumes(
        to_point2(input_polyline_axes),  # input_polylines,
        to_double2(input_polylines_segment_radii),
        to_vector2(input_polylines_segment_direction),  # input_polylines_segment_direction,
        to_int1(input_allowed_types_per_polyline),
        float(input_min_distance),
        float(input_volume_length),
        float(input_cross_or_side_to_end),
        int(input_flip_male),
        index_polylines,  # out
        index_polylines_segment,  # out
        distance,  # out
        point_pairs,  # out
        volume_pairs,  # out
        joints_areas,  # out
        joints_types,  # out
        w_output_plines,  # from_point3(w_output_plines)
        w_output_types,  # from_cut_type2(w_output_types))
        input_compute_joints,
        input_division_distance,
        input_shift,
        input_output_geometry_type,
        input_use_eccentricities_to_scale_joints,
    )

    print(index_polylines)

    def from_point1(point1):
        points = []
        for i in range(len(point1)):
            point = point1[i]
            points.append(Point3d(point[0], point[1], point[2]))
        return Polyline(points)

    def from_point2(point2):
        return [from_point1(p) for p in point2]

    def from_point3(point3):
        return [from_point2(p) for p in point3]

    output_volume_pairs = from_point3(volume_pairs)
    output_volume_pairs_flat = []

    for idx, o in enumerate(output_volume_pairs):
        output_volume_pairs_flat.extend(o)

    group_indices = []
    for polylines_indices in index_polylines:
        for index in polylines_indices:
            group_indices.append(index)
            group_indices.append(index)
            print(index)

    add_polylines(output_volume_pairs_flat, dataset_name, group_indices)

    shapes = []
    for axis in input_polyline_axes:
        localblending = False
        cap = Rhino.Geometry.PipeCapMode.Flat
        fitrail = True
        abstol = 0.1
        import math

        angtol = math.radians(15)
        shape = list(
            Rhino.Geometry.Brep.CreatePipe(
                axis.ToNurbsCurve(), input_radii, localblending, cap, fitrail, abstol, angtol
            )
        )[0]
        shapes.append(shape)

    add_axes(input_polyline_axes, dataset_name, group_indices, shapes)

    # return (
    #     from_int2(index_polylines),
    #     from_int2(index_polylines_segment),
    #     from_double2(distance),
    #     from_point2(point_pairs),
    #     from_point3(volume_pairs),
    #     from_point2(joints_areas),
    #     from_int1(joints_types),
    # )


if __name__ == "__main__":

    # Define the input dictionary based on your initial dataset
    input_dict = {
        "polyline_axes": ([], List[Rhino.Geometry.Polyline]),  # Default value for polylines (list of polylines)
        "polylines_segment_radii": (50, float),  # Default value for radii (float)
        "polylines_segment_direction": (
            [],
            List[Rhino.Geometry.Polyline],
        ),  # Default value for directions (list of lines)
        "allowed_types_per_polyline": ([-1], List[int]),  # Default value for types (list of ints)
        "min_distance": (50, float),  # Default value for min distance (float)
        "volume_length": (300, float),  # Default value for volume length (float)
        "cross_or_side_to_end": (0.91, float),  # Default value for cross or side to end (float)
        # "flip_male": (1, int),  # Default value for flip
        # "compute_joints": (False, bool),  # Default value for compute joints
        # "division_distance": (150, float),  # Default value for division distance
        # "shift": (0.5, float),  # Default value for shift
        # "output_geometry_type": (0, int),  # Default value for output geometry type
        # "use_eccentricities_to_scale_joints": (True, bool),  # Default value for use eccentricities to scale joints
    }

    # Call the generalized input method with the dataset name and input dictionary
    dataset_name = "beams"
    wood_rui_globals.init_data(dataset_name)
    print(wood_rui_globals)
    generalized_input_method(dataset_name, input_dict, my_callback)
