class WoodData(object):
    def __init__(
        self,
        plines=[],
        insertion_vectors=[],
        joints_per_face=[],
        three_valence=[],
        adjacency=[],
        planes=[],
        plines_with_joints=[],
        cut_types=[],
    ):
        self.plines = plines
        self.insertion_vectors = insertion_vectors
        self.joints_per_face = joints_per_face
        self.three_valence = three_valence
        self.adjacency = adjacency
        self.planes = planes
        self.plines_with_joints = plines_with_joints
        self.cut_types = cut_types
