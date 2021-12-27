# compas_wood

Joinery Solver


## Create Package

### Software needed:
Anaconda
VSCode
Github Desktop
Git for windows, apple has by default

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
Default documentation from template using sphynx
```
cookiecutter gh:compas-dev/tpl-extension
```
Fill basic form and rename current branch
```
cd compas_wood
git branch -m main
git add .
git commit -m "Initial commit"
```
Install - build project in the correct environment and cd must be the good directory
```
pip install -r requirements-dev.txt
invoke docs
pip install -e .
```

### Step 3
Instal Compas
```
conda install compas
invoke docs
```

### Step 4
invoke release patch or minor major 0.0.1 (new version / backwards compatible / bugs fixes)
```
invoke release patch 
```






https://user-images.githubusercontent.com/18013985/147421244-b1aaa4bb-9b8b-429a-b9c8-4335cc853cd9.mp4

## Pybind11 C++/Python CGAL

### Part1

Test simple pybind11 example to check the connection between C++ and Python.
Files attached bellow contains: a) setup file, b) C++ file in a src folder and c) python file in a test folder.
[pybind11_hello_world.zip](https://github.com/petrasvestartas/compas_wood/files/7781554/pybind11_hello_world.zip)

Following video explains how to change default files with the ones aboce
https://user-images.githubusercontent.com/18013985/147499175-f53dc798-2e96-4f05-8482-03b58136f786.mp4


### Part2

Joinery Solver C++ project that includes pybind11 bindings in xxx_interop_python source files.
Similarly to the part 1, change: a) setup file, b) source folder with C++ files and c) test folder with python files.
[pybind11_joinery_solver.zip](https://github.com/petrasvestartas/compas_wood/files/7781555/pybind11_joinery_solver.zip)

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
