# Contributing

Contributions are welcome and very much appreciated!

## Commit Rules:

    Two keywors: <type_of_commit> <project_name> <description>
    <type_of_commit> ADD FIX MISC REM
    <project_name> BACKEND FRONTEND COMPAS
    e.g. ADD BACKEND origami folding surface generation

## Commit Sequence:

    git add --all
    git commit -m "<type_of_commit> <project_name> <description>"
    git push

# Conda

## Create Environment:

### Windows

    conda create -n wood-dev python=3.8 mpir mpfr boost-cpp eigen=3.4 cgal-cpp=5.5 pybind11 compas compas_view2 --yes
     conda create -n wood-dev python=3.8 mpir mpfr boost-cpp eigen=3.4 pybind11 compas compas_view2 --yes
    conda activate wood-dev
    git clone https://github.com/petrasvestartas/compas_wood
    cd compas_wood
    pip install -e .

### Mac


    conda create -n wood-dev python=3.8 gmp mpfr boost-cpp eigen=3.4 cgal-cpp=5.5 pybind11 compas compas_view2 --yes
    conda create -n wood-dev python=3.8 gmp mpfr boost-cpp eigen=3.4 pybind11 compas compas_view2 --yes
    conda activate wood-dev
    git clone https:/github.com/petrasvestartas/compas_wood
    cd compas_wood 
    pip install -e . 

## Remove Environment:

    conda remove -n wood-dev --all


## Bug reports

When [reporting a bug](https://github.com/compas-dev/compas_wood/issues) please include:

* Operating system name and version.
* Any details about your local setup that might be helpful in troubleshooting.
* Detailed steps to reproduce the bug.

## Feature requests

When [proposing a new feature](https://github.com/compas-dev/compas_wood/issues) please include:

* Explain in detail how it would work.
* Keep the scope as narrow as possible, to make it easier to implement.

## Code contributions

Instructions for code contributions will be available soon.
