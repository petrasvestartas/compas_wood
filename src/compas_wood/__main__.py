from __future__ import print_function

import platform
import compas_wood


if __name__ == "__main__":
    print()
    print("Yay! COMPAS WOOD is installed correctly!")
    print()
    print("COMPAS WOOD: {}".format(compas_wood.__version__))
    print("Python: {} ({})".format(platform.python_version(), platform.python_implementation()))
