"""
Do something silly in python3.

This component does nothing useful, it's only a minimal example of python3 in grasshopper.

    Args:
        x: X value
        y: Y value
        z: Z value
    Returns:
        a: The sum of all three values.
"""
import platform

ghenv.Component.Message = 'COMPONENT v{{version}}'

a = x + y + z
