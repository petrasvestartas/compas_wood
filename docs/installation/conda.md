# Conda Installation

## Create Environment

```bash
conda create -n compas_wood_3_9_10 -c conda-forge python=3.9.10 compas
conda activate compas_wood_3_9_10
```

## Clone and Install

```bash
git clone https://github.com/petrasvestartas/compas_wood.git
cd compas_wood
pip install -r requirements.txt
pip install -e .
```

## Optional - Viewer

```bash
pip install compas_viewer==1.1.2
```

## Visual Studio Code Setup

1. Launch `VSCode` and select the `compas_wood_3_9_10` Python environment using `CTRL+SHIFT+P`
2. Open a new terminal via `Terminal -> New Terminal`
3. Navigate to the `docs/examples` folder and execute any `.py` example file

![VSCode Environment](../assets/images/vscode_environment.gif)

## Prerequisites

If you are new to Anaconda, install these tools first:

- [Anaconda](https://www.anaconda.com/download)
- [Visual Studio Code](https://code.visualstudio.com/download)
- [Git](https://git-scm.com/downloads)
