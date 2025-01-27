from wood_nano import test as wood_nano_test


def test():
    """Test if wood_nano package is loaded by printing message in C++."""
    wood_nano_test()


if __name__ == "__main__":
    wood_nano_test()
