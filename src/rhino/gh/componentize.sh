#!/bin/bash

# Install pythonnet
pip install pythonnet

# Run the desired command
python src/rhino/gh/componentize_cpy.py src/rhino/gh/cpy src/rhino/gh/build

# Run the componentize_cpy.py script for the user objects directory
python src/rhino/gh/componentize_cpy.py src/rhino/gh/cpy  "/Users/petras/Library/Application Support/McNeel/Rhinoceros/8.0/Plug-ins/Grasshopper (b45a29b1-4343-4035-989e-044e8580d9cf)/UserObjects"
