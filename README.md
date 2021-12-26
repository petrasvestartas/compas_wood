# compas_wood

Joinery Solver


## Create Package

### Software needed:
Anaconda
VSCode
Github Desktop
Git for windows, apple has by default

### Step 1
//Create new environment:
```
conda create -n wood python=3.8 --yes
```
//or
windows
% conda create -n wood python=3.8 mpir mpfr boost-cpp eigen=3.3 cgal-cpp=5.2 pybind11 compas compas_view2 --yes
conda activate wood
git clone https://github.com/compas-dev/compas_cgal
cd compas_cgal
pip install -e .

mac
% conda create -n wood python=3.8 gmp mpfr boost-cpp eigen=3.3 cgal-cpp=5.2 pybind11 compas compas_view2 --yes
conda activate wood or wood-dev

### Step 2
pip install cookiecutter
//Default documentation from template using sphynx
cookiecutter gh:compas-dev/tpl-extension
//Fill basic infom
//Rename current branch
cd compas_wood
git branch -m main
git add .
git commit -m "Initial commit"
//Install 
pip install -r requirements-dev.txt
invoke docs
pip install -e .

### Step 3
#Instal Compas
conda install compas
invoke docs

### Step 4
invoke release patch or minor major 0.0.1 (new version / backwards compatible / bugs fixes)
Build project in the correct environment and cd must be the good directory

Good python repos: Open Cascade, OCC, GMesh

Delete env 
conda env remove --name bio-env



https://user-images.githubusercontent.com/18013985/147421244-b1aaa4bb-9b8b-429a-b9c8-4335cc853cd9.mp4

## Pybind11 C++/Python CGAL

