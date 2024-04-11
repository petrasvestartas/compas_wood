# compas_wood

timber joinery generation

- activate correct environment: export PATH="~/anaconda3/envs/compas_wood/bin:$PATH"


## Getting started with this project

### Setup code editor

1. Open project folder in VS Code
2. Select python environment for the project
3. First time using VS Code and on Windows? Make sure select the correct terminal profile: `Ctrl+Shift+P`, `Terminal: Select Default Profile` and select `Command Prompt`.

> All terminal commands in the following sections can be run from the VS Code integrated terminal. 


### First steps with git

1. Go to the `Source control` tab
2. Make an initial commit with all newly created files


### First steps with code

1. Install the newly created project 

        pip install -e .

2. Install it on Rhino

        python -m compas_rhino.install


### Code conventions

Code convention follows [PEP8](https://pep8.org/) style guidelines and line length of 120 characters.

1. Check adherence to style guidelines

        invoke lint

2. Format code automatically

        invoke format


### Documentation

Documentation is generated automatically out of docstrings and [RST](https://www.sphinx-doc.org/en/master/usage/restructuredtext/basics.html) files in this repository

1. Generate the docs

        invoke docs

2. Check links in docs are valid

        invoke linkcheck

3. Open docs in your browser (file explorer -> `dist/docs/index.html`)


### Testing

Tests are written using the [pytest](https://docs.pytest.org/) framework

1. Run all tests from terminal

        invoke test

2. Or run them from VS Code from the `Testing` tab


### Developing Grasshopper components

We use [Grasshopper Componentizer](https://github.com/compas-dev/compas-actions.ghpython_components) to develop Python components that can be stored and edited on git.

1. Build components

        invoke build-ghuser-components

2. Install components on Rhino

        python -m compas_rhino.install


### Publish release

Releases follow the [semver](https://semver.org/spec/v2.0.0.html) versioning convention.

1. Create a new release

        invoke release major
