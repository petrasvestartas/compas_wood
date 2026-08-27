"""Named JSON datasets shipped with wood_nano, converted to compas geometry."""

from compas.geometry import Polyline
from wood_nano.datasets import DATASETS_DIR
from wood_nano.datasets import load_dataset as _load_dataset_raw


def list_datasets() -> list[str]:
    """Sorted names of the datasets shipped with wood_nano (JSON file stems)."""
    return sorted(p.stem for p in DATASETS_DIR.glob("*.json"))


def load_dataset(name: str) -> tuple[list[Polyline], list[Polyline], dict]:
    """Load a named dataset → (bottom_polylines, top_polylines, params).

    Plate outlines in the shipped data are already closed (first == last point);
    loops of 3+ points are closed here if not. 2-point polylines (beam-axis
    datasets) are segments and stay open. params is the raw dict from
    wood_nano.datasets.load_dataset, passed through unchanged.
    """

    def _to_polyline(pts: list) -> Polyline:
        pts = [[float(p[0]), float(p[1]), float(p[2])] for p in pts]
        if len(pts) > 2 and pts[0] != pts[-1]:
            pts.append(pts[0])
        return Polyline(pts)

    bottom, top, params = _load_dataset_raw(name)
    return [_to_polyline(pl) for pl in bottom], [_to_polyline(pl) for pl in top], params
