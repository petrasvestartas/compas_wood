# compas_wood

Joinery Solver


## Create Package

### Software needed:
* Anaconda
* VSCode
* Github Desktop
* Git for windows, mac has git installed by default

### Step 1
Create new environment:
```
conda create -n wood python=3.8 --yes
conda activate wood
```
windows
``` 
conda create -n wood python=3.8 mpir mpfr boost-cpp eigen=3.3 cgal-cpp=5.2 pybind11 compas compas_view2 --yes 
conda activate wood
```
mac
```
conda create -n wood python=3.8 gmp mpfr boost-cpp eigen=3.3 cgal-cpp=5.2 pybind11 compas compas_view2 --yes
conda activate wood
```


### Step 2

Install CookieCutter
```
pip install cookiecutter
```

Install compas form for documentation template using spynx,  fill the basic form with your credentials
```
cookiecutter gh:compas-dev/tpl-extension
```

Rename current branch to main and perform first commit
```
cd compas_wood
git branch -m main
git add .
git commit -m "Initial commit"
```

Build project (in the correct environment and cd must be in the good directory)
```
pip install -r requirements-dev.txt
invoke docs
pip install -e .
```

### Step 3
Install Compas, if not installed before. And build documentataion.
```
conda install compas
invoke docs
```

### Step 4
Create a release 0.0.x (new version / backwards compatible / bugs fixes)

```
invoke release patch 
```
or  0.x.0
```
invoke release minor
```
or x.0.0
```
invoke release major
```






https://user-images.githubusercontent.com/18013985/147421244-b1aaa4bb-9b8b-429a-b9c8-4335cc853cd9.mp4

## Pybind11 C++/Python CGAL

### Part1 [pybind11_hello_world.zip](https://github.com/petrasvestartas/compas_wood/files/7781554/pybind11_hello_world.zip)

Test simple pybind11 example to check the connection between C++ and Python.
Files attached contain: 
* setuptools instructions (setup.py file)
* C++ source code (src folder) with C++ files
* python example files (test folder).


Following video explains how to change default files with the ones above:

https://user-images.githubusercontent.com/18013985/147499175-f53dc798-2e96-4f05-8482-03b58136f786.mp4


### Part2 [pybind11_joinery_solver.zip](https://github.com/petrasvestartas/compas_wood/files/7781555/pybind11_joinery_solver.zip)

Joinery Solver C++ project that includes pybind11 bindings in xxx_interop_python source files.
Similarly to the part 1, change
* setuptools instructions (setup.py file)
* C++ source code (src folder) with C++ files. Pybind11 files are xxx_interop_python.cpp and xxx_interop_python.h files.
* python example files (test folder). Simple import case in test_compas_wood.py, full wrapper in connection_detection.py and its test in test_connectionDetection.py.


Following video explains how to change default files with the ones above:

https://user-images.githubusercontent.com/18013985/147499189-e45a4160-94dd-4d66-849f-5cfedd8f5e7d.mp4


## Clone Repository

```
conda activate wood
git clone https://github.com/compas-dev/compas_cgal
cd compas_cgal
pip install -e .
```

___
Other Resources:
Good python repos: Open Cascade, OCC, GMesh
Delete env ```conda env remove --name bio-env ```
