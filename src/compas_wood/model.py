"""In-memory plate topology: the compas replacement for the Rhino-only wood_nano PlateTopology.

Plates, per-plate joinery metadata (joint_types, insertion_vectors) and global chevron
joinery data (three_valence, adjacency) live on plain compas Data objects instead of
Rhino UserStrings / document strings, so the whole model json round-trips.
"""

import warnings

from compas.data import Data
from compas.datastructures import Mesh
from compas.geometry import Polyline

from compas_wood.joinery_solver import joinery_solver_elements


def _unweld_mesh(mesh: Mesh) -> Mesh:
    """Duplicate vertices per face (flat shading), like the plugin's unweld_mesh."""
    vertices: list[list[float]] = []
    faces: list[list[int]] = []
    for fkey in mesh.faces():
        face = []
        for v in mesh.face_vertices(fkey):
            face.append(len(vertices))
            vertices.append(mesh.vertex_coordinates(v))
        faces.append(face)
    return Mesh.from_vertices_and_faces(vertices, faces)


class Plate(Data):
    """One plate element: bottom/top outlines, optional loft mesh, holes, and joinery metadata.

    Parameters
    ----------
    plate_id : int
    bottom, top : compas.geometry.Polyline
    mesh : compas.datastructures.Mesh, optional
    holes_bottom, holes_top : list[compas.geometry.Polyline], optional
        Parallel per-hole outlines; lengths must match.
    joint_types : list[int], optional
        Joint-type code per face slot (n_faces entries; slots 0/1 = bottom/top faces).
    insertion_vectors : list[float], optional
        18 floats = 6 Vec3 insertion directions.
    plate_type : str
        Logical tag, e.g. "plate" / "face" / "edge".
    """

    def __init__(
        self,
        plate_id: int,
        bottom: Polyline,
        top: Polyline,
        mesh: Mesh | None = None,
        holes_bottom: list[Polyline] | None = None,
        holes_top: list[Polyline] | None = None,
        joint_types: list[int] | None = None,
        insertion_vectors: list[float] | None = None,
        plate_type: str = "plate",
        name: str | None = None,
    ):
        super().__init__(name=name)
        holes_bottom = list(holes_bottom) if holes_bottom else []
        holes_top = list(holes_top) if holes_top else []
        if len(holes_bottom) != len(holes_top):
            raise ValueError(
                f"plate {plate_id}: {len(holes_bottom)} bottom holes vs "
                f"{len(holes_top)} top holes - pairing would silently drop the extras."
            )
        self.plate_id = int(plate_id)
        self.bottom = bottom
        self.top = top
        self.mesh = mesh
        self.holes_bottom = holes_bottom
        self.holes_top = holes_top
        self.joint_types = [int(x) for x in joint_types] if joint_types else []
        self.insertion_vectors = [float(x) for x in insertion_vectors] if insertion_vectors else []
        self.plate_type = plate_type

    @property
    def n_edges(self) -> int:
        pts = self.bottom.points if self.bottom is not None else []
        if len(pts) >= 3:
            return len(pts) - 1 if pts[0].distance_to_point(pts[-1]) < 1e-6 else len(pts)
        return 4

    @property
    def n_faces(self) -> int:
        # slots 0/1 = bottom/top faces, slots 2+ = side faces.
        return self.n_edges + 2

    @property
    def __data__(self) -> dict:
        return {
            "plate_id": self.plate_id,
            "bottom": self.bottom,
            "top": self.top,
            "mesh": self.mesh,
            "holes_bottom": self.holes_bottom,
            "holes_top": self.holes_top,
            "joint_types": self.joint_types,
            "insertion_vectors": self.insertion_vectors,
            "plate_type": self.plate_type,
            "name": self.name,
        }

    @classmethod
    def __from_data__(cls, data: dict) -> "Plate":
        return cls(**data)


class PlateModel(Data):
    """Plate collection plus joinery metadata; feeds :func:`joinery_solver_elements` via solver_kwargs."""

    def __init__(self, name: str | None = None):
        super().__init__(name=name)
        self.plates: dict[int, Plate] = {}
        self.three_valence: list[list[int]] = []
        self.adjacency: list[list[int]] = []

    # ------------------------------------------------------------------
    # Write
    # ------------------------------------------------------------------

    def clear(self) -> None:
        self.plates.clear()
        self.three_valence = []
        self.adjacency = []

    def add_plate(
        self,
        plate_id: int,
        bottom: Polyline,
        top: Polyline,
        mesh: Mesh | None = None,
        holes_bottom: list[Polyline] | None = None,
        holes_top: list[Polyline] | None = None,
        plate_type: str = "plate",
    ) -> Plate:
        plate = Plate(
            plate_id,
            bottom,
            top,
            mesh=mesh,
            holes_bottom=holes_bottom,
            holes_top=holes_top,
            plate_type=plate_type,
        )
        self.plates[plate.plate_id] = plate
        return plate

    def tag_plate_joinery(self, plate_id: int, joint_types: list[int], insertion_vectors: list[float]) -> None:
        plate = self.plates[int(plate_id)]
        plate.joint_types = [int(x) for x in (joint_types or [])]
        plate.insertion_vectors = [float(x) for x in (insertion_vectors or [])]

    def set_global_joinery(self, three_valence: list[list[int]], adjacency: list[list[int]]) -> None:
        self.three_valence = [[int(x) for x in tv] for tv in (three_valence or [])]
        self.adjacency = [[int(a), int(b)] for a, b in (adjacency or [])]

    # ------------------------------------------------------------------
    # Query
    # ------------------------------------------------------------------

    def get_global_joinery(self) -> tuple[list[list[int]], list[list[int]]]:
        return [list(tv) for tv in self.three_valence], [list(ab) for ab in self.adjacency]

    def get_plate_joinery(self, plate_id: int) -> tuple[list[int], list[float]]:
        plate = self.plates.get(int(plate_id))
        if plate is None:
            return [], []
        return list(plate.joint_types), list(plate.insertion_vectors)

    def plate_ids(self) -> list[int]:
        return sorted(self.plates)

    def subset(self, plate_ids: list[int]) -> "PlateModel":
        """New model with only the given plates; keeps original ids and full global metadata."""
        model = PlateModel(name=self._name)
        for pid in sorted({int(p) for p in plate_ids}):
            if pid in self.plates:
                model.plates[pid] = self.plates[pid]
        model.three_valence = [list(tv) for tv in self.three_valence]
        model.adjacency = [list(ab) for ab in self.adjacency]
        return model

    # ------------------------------------------------------------------
    # Constructors
    # ------------------------------------------------------------------

    @classmethod
    def from_elements(cls, elements, loft_meshes=None, joint_data=None, unweld=False) -> "PlateModel":
        """From compas_wood WoodElement objects, optionally with generator loft meshes and chevron joint_data."""
        model = cls()
        for i, el in enumerate(elements):
            mesh = None
            if loft_meshes is not None and i < len(loft_meshes):
                mesh = loft_meshes[i]
                if unweld and mesh is not None:
                    mesh = _unweld_mesh(mesh)
            model.add_plate(i, el.bottom, el.top, mesh=mesh)
        if joint_data:
            jpf = joint_data.get("joints_per_face") or []
            ivs = joint_data.get("insertion_vectors") or []
            for i in range(len(elements)):
                jt = list(jpf[i]) if i < len(jpf) else []
                iv = list(ivs[i]) if i < len(ivs) else []
                if jt or iv:
                    model.tag_plate_joinery(i, jt, iv)
            model.set_global_joinery(joint_data.get("three_valence") or [], joint_data.get("adjacency") or [])
        return model

    @classmethod
    def from_beams(cls, beams, side0, side1) -> "PlateModel":
        if not (len(beams) == len(side0) == len(side1)):
            raise ValueError(
                f"beams ({len(beams)}), side0 ({len(side0)}) and side1 ({len(side1)}) must have the same length."
            )
        model = cls()
        for i, (mesh, bot, top) in enumerate(zip(beams, side0, side1)):
            model.add_plate(i, bot, top, mesh=mesh)
        return model

    @classmethod
    def from_polylines(cls, bottom, top) -> "PlateModel":
        if len(bottom) != len(top):
            raise ValueError(f"bottom ({len(bottom)}) and top ({len(top)}) must have the same length.")
        model = cls()
        for i, (bot, tp) in enumerate(zip(bottom, top)):
            model.add_plate(i, bot, tp)
        return model

    @classmethod
    def from_connectors(cls, f_polylines, e_polylines, loft=True, unweld=True) -> "PlateModel":
        """From connectors_elements output rows of alternating bot/top polylines (stride 2)."""
        model = cls()
        loft_fn = None
        if loft:
            try:
                from compas_wood.loft import loft as loft_fn
            except ImportError:
                loft_fn = None  # loft module absent: plates keep mesh=None

        def _mesh(bot, top):
            if loft_fn is None:
                return None
            mesh = loft_fn([bot], [top])
            return _unweld_mesh(mesh) if unweld and mesh is not None else mesh

        plate_id = 0
        for row in f_polylines:
            for j in range(0, len(row) - 1, 2):
                bot, top = row[j], row[j + 1]
                if len(bot.points) < 2:
                    continue
                model.add_plate(plate_id, bot, top, mesh=_mesh(bot, top), plate_type="face")
                plate_id += 1
        for row in e_polylines:
            for j in range(0, len(row) - 1, 2):
                bot, top = row[j], row[j + 1]
                if len(bot.points) < 2 or len(top.points) < 2:
                    continue
                model.add_plate(plate_id, bot, top, mesh=_mesh(bot, top), plate_type="edge")
                plate_id += 1
        return model

    @classmethod
    def from_breps(cls, breps, tol: float = 1e-6, skip_invalid: bool = False) -> "PlateModel":
        from compas_wood.brep import plate_from_brep

        model = cls()
        skipped = 0
        pid = 0
        for brep in breps:
            try:
                plate = plate_from_brep(brep, pid, tol=tol)
            except ValueError:
                # non-plate solid (dowel, cylinder, connector) - only tolerated when asked
                if not skip_invalid:
                    raise
                skipped += 1
                continue
            model.plates[int(plate.plate_id)] = plate
            pid += 1
        if skipped:
            warnings.warn(f"from_breps: skipped {skipped} non-plate solid(s) of {len(breps)}.", stacklevel=2)
        return model

    # ------------------------------------------------------------------
    # Solver
    # ------------------------------------------------------------------

    def solver_kwargs(
        self,
        search_type: int = 0,
        joint_params: list | None = None,
        joint_volume_ext: list | None = None,
        plate_ids=None,
    ) -> dict:
        """Keyword arguments for :func:`joinery_solver_elements`, with the plugin's exact gating.

        Skips plates missing bottom or top, remaps stored plate ids to accepted indices,
        enforces the C++ row contracts (std::array<double,18> / std::array<int,6>), and
        passes joinery metadata only when explicit data exists.
        """
        candidates = sorted({int(p) for p in plate_ids}) if plate_ids is not None else self.plate_ids()

        bottom_pls: list = []
        top_pls: list = []
        bottom_hole_pls: list = []
        top_hole_pls: list = []
        per_element_jt: list = []
        per_element_iv: list = []
        accepted: list[int] = []
        for pid in candidates:
            plate = self.plates.get(pid)
            if plate is None or plate.bottom is None or plate.top is None:
                missing = "plate" if plate is None else ("bottom" if plate.bottom is None else "top")
                warnings.warn(f"plate {pid}: missing {missing} - skipped.")
                continue
            bottom_pls.append(plate.bottom)
            top_pls.append(plate.top)
            bottom_hole_pls.append(list(plate.holes_bottom))
            top_hole_pls.append(list(plate.holes_top))
            jt, iv = self.get_plate_joinery(pid)
            per_element_jt.append(jt)
            per_element_iv.append(iv)
            accepted.append(pid)

        if not bottom_pls:
            raise ValueError("no complete plate pairs - every plate needs both bottom and top polylines.")

        # Skipped plates shift indices: remap stored plate ids, drop pairs/groups referencing skipped ones.
        pid_to_idx: dict[int, int] = {pid: idx for idx, pid in enumerate(accepted)}
        adjacency_data = [
            [pid_to_idx[a], pid_to_idx[b]] for a, b in self.adjacency if a in pid_to_idx and b in pid_to_idx
        ]
        three_valence_data = [
            [pid_to_idx[x] for x in tv] for tv in self.three_valence if all(x in pid_to_idx for x in tv)
        ]

        has_explicit_jt = any(row and any(x >= 0 for x in row) for row in per_element_jt)
        has_nonzero_iv = any(row and any(abs(x) > 1e-6 for x in row) for row in per_element_iv)
        has_explicit_data = has_explicit_jt or has_nonzero_iv or bool(three_valence_data) or bool(adjacency_data)

        if has_explicit_data:
            # C++ contracts: std::array<double,18> / std::array<int,6> - non-empty rows must be exact.
            for i, pid in enumerate(accepted):
                if per_element_iv[i] and len(per_element_iv[i]) != 18:
                    warnings.warn(
                        f"plate {pid}: insertion_vectors has {len(per_element_iv[i])} floats, "
                        "expected exactly 18 - row dropped."
                    )
                    per_element_iv[i] = []
                if per_element_jt[i] and len(per_element_jt[i]) != 6:
                    warnings.warn(
                        f"plate {pid}: joint_types has {len(per_element_jt[i])} ints, expected exactly 6 - row dropped."
                    )
                    per_element_jt[i] = []
            # jt=None when only iv is set - an all-zero joint-type array blocks joint creation in C++.
            if has_explicit_jt:
                per_element_jt = [
                    [max(0, x) for x in row] if (row and any(x >= 0 for x in row)) else [] for row in per_element_jt
                ]
            else:
                per_element_jt = []

        has_holes = any(h for h in bottom_hole_pls)
        return {
            "bottom_polylines": bottom_pls,
            "top_polylines": top_pls,
            "search_type": int(search_type),
            "joint_params": joint_params if joint_params else None,
            "joint_volume_ext": joint_volume_ext if joint_volume_ext else None,
            "per_element_insertion_vectors": per_element_iv
            if (has_nonzero_iv or bool(adjacency_data) or bool(three_valence_data))
            else None,
            "per_element_joint_types": (per_element_jt or None) if has_explicit_data else None,
            "three_valence": three_valence_data if has_explicit_data else None,
            "adjacency": adjacency_data if has_explicit_data else None,
            "bottom_hole_polylines": bottom_hole_pls if has_holes else None,
            "top_hole_polylines": top_hole_pls if has_holes else None,
        }

    def solve(
        self,
        search_type: int = 0,
        joint_params: list | None = None,
        joint_volume_ext: list | None = None,
        plate_ids=None,
    ) -> tuple[list, list]:
        """Run the joinery solver on this model; returns (elements, joints)."""
        return joinery_solver_elements(
            **self.solver_kwargs(
                search_type=search_type,
                joint_params=joint_params,
                joint_volume_ext=joint_volume_ext,
                plate_ids=plate_ids,
            )
        )

    # ------------------------------------------------------------------
    # Serialization
    # ------------------------------------------------------------------

    @property
    def __data__(self) -> dict:
        return {
            "plates": [self.plates[pid] for pid in self.plate_ids()],
            "three_valence": [list(tv) for tv in self.three_valence],
            "adjacency": [list(ab) for ab in self.adjacency],
            "name": self.name,
        }

    @classmethod
    def __from_data__(cls, data: dict) -> "PlateModel":
        model = cls(name=data.get("name"))
        for plate in data["plates"]:
            model.plates[plate.plate_id] = plate
        model.three_valence = [list(tv) for tv in data.get("three_valence", [])]
        model.adjacency = [list(ab) for ab in data.get("adjacency", [])]
        return model
