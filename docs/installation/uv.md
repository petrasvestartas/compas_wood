# UV Installation

[UV](https://github.com/astral-sh/uv) is a modern, fast Python package manager written in Rust.

## Install UV

```bash
# Windows (using pip)
pip install uv

# Windows (using winget)
winget install --id=astral-sh.uv -e

# macOS/Linux
curl -LsSf https://astral.sh/uv/install.sh | sh
```

## Create Project with UV

```bash
# Create a new project
uv init my_wood_project
cd my_wood_project

# Add compas_wood as dependency
uv add compas_wood

# Run your script
uv run python your_script.py
```

## Virtual Environment with UV

```bash
# Create virtual environment
uv venv

# Activate (Windows)
.venv\Scripts\activate

# Activate (macOS/Linux)
source .venv/bin/activate

# Install compas_wood
uv pip install compas_wood
```

## Development Setup with UV

```bash
git clone https://github.com/petrasvestartas/compas_wood.git
cd compas_wood
uv venv
uv pip install -e ".[dev]"
```

!!! tip
    UV is significantly faster than pip and conda for package resolution and installation.
