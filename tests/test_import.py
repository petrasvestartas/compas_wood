import compas_wood


def test_all_names_resolve():
    assert compas_wood.__all__
    for name in compas_wood.__all__:
        assert hasattr(compas_wood, name), f"compas_wood.{name} missing"


def test_version():
    assert compas_wood.__version__ == "3.0.0"


def test_datasets_dir_exists():
    assert compas_wood.DATASETS_DIR.exists()
    assert compas_wood.DATASETS_DIR.is_dir()
