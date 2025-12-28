# Rhino Installation

## Package Manager Installation

1. Open Rhino 8
2. In the Rhino command-line type: `PackageManager`
3. In the PackageManager search for `compas_wood`
4. Click the **Install** button
5. Restart Rhino 8
6. Open Grasshopper
7. Drag and drop the install component
8. Try examples from the Food4Rhino page

![Grasshopper Installation](../assets/images/grasshopper_installation.gif)

## Examples

- [Grasshopper Examples](https://github.com/petrasvestartas/compas_wood/tree/main/src/rhino/gh/examples)

## Verify Installation

Run this test script in Rhino's ScriptEditor:

1. Open "ScriptEditor"
2. Create a new "Python 3 script"
3. Run the following code:

```python
import compas_wood
print(compas_wood.__version__)
```

!!! attention
    If there are errors, contact petrasvestartas@gmail.com with your Rhino version and OS details.
