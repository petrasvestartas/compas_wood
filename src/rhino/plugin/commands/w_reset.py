#! python3
# venv: timber_connections
from wood_rui import wood_rui_globals  # Import the singleton instance

if __name__ == "__main__":
    wood_rui_globals.reset()
    print(wood_rui_globals)
