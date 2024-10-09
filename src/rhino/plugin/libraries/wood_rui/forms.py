import ast
from pathlib import Path
import Eto.Drawing  # type: ignore
import Eto.Forms  # type: ignore
import Rhino  # type: ignore
import Rhino.UI  # type: ignore
import System  # type: ignore


class AboutForm:
    def __init__(
        self,
        title,  # type: str
        description,  # type: str
        version,  # type: str
        website,  # type: str
        copyright,  # type: str
        license,  # type: str
        designers=None,  # type: list[str] | None
        developers=None,  # type: list[str] | None
        documenters=None,  # type: list[str] | None
    ):
        # type: (...) -> None
        designers = designers or []
        developers = developers or []
        documenters = documenters or []

        self.dialog = Eto.Forms.AboutDialog()
        self.dialog.Copyright = copyright
        self.dialog.Designers = System.Array[System.String](designers)
        self.dialog.Developers = System.Array[System.String](developers)
        self.dialog.Documenters = System.Array[System.String](documenters)
        self.dialog.License = license
        self.dialog.ProgramDescription = description
        self.dialog.ProgramName = title
        self.dialog.Title = title
        self.dialog.Version = version
        self.dialog.Website = System.Uri(website)

    def show(self):
        self.dialog.ShowDialog(Rhino.UI.RhinoEtoApp.MainWindow)

class NamedValuesForm(Eto.Forms.Dialog[bool]):
    """Eto form for displaying and editing named values.

    Parameters
    ----------
    names : list[str]
        The names of the values.
    values: list[str, byte, float, int, tuple, list, dict, set, bool, None]
        The values.
        Only literal Python structures are allowed.
        Other types of values are ignored.
    title : str, optional
        The title of the form window.
    width : int, optional
        The width of the form window.
    height : int, optional
        The height of the form window.
    """

    def __init__(
        self,
        names,  # type: list[str]
        values,  # type: list
        title="Named Values",  # type: str
        width=600,  # type: int
        height=800,  # type: int
    ):
        # type: (...) -> None
        super().__init__()

        def on_cell_formatting(sender, e):
            try:
                if not e.Column.Editable:
                    e.ForegroundColor = Eto.Drawing.Colors.DarkGray
            except Exception as e:
                print(e)

        # Store as a list of tuples (allowing duplicates)
        self.attributes = list(zip(names, values))
        self.names = names
        self.values = [str(value) for value in values]

        self.Title = title
        self.Padding = Eto.Drawing.Padding(0)
        self.Resizable = True
        self.MinimumSize = Eto.Drawing.Size(0.5 * width, 0.5 * height)
        self.ClientSize = Eto.Drawing.Size(width, height)

        self.table = table = Eto.Forms.GridView()
        table.ShowHeader = True

        column = Eto.Forms.GridColumn()
        column.HeaderText = "Name"
        column.Editable = False
        column.DataCell = Eto.Forms.TextBoxCell(0)
        table.Columns.Add(column)

        column = Eto.Forms.GridColumn()
        column.HeaderText = "Value"
        column.Editable = True
        column.DataCell = Eto.Forms.TextBoxCell(1)
        table.Columns.Add(column)

        collection = []
        for name, value in zip(self.names, self.values):
            item = Eto.Forms.GridItem()
            item.Values = (name, value)
            collection.append(item)
        table.DataStore = collection

        layout = Eto.Forms.DynamicLayout()
        layout.BeginVertical(Eto.Drawing.Padding(0, 0, 0, 0), Eto.Drawing.Size(0, 0), True, True)
        layout.AddRow(table)
        layout.EndVertical()
        layout.BeginVertical(Eto.Drawing.Padding(12, 18, 12, 24), Eto.Drawing.Size(6, 0), False, False)
        layout.AddRow(None, self.ok, self.cancel)
        layout.EndVertical()

        self.Content = layout

    @property
    def ok(self):
        self.DefaultButton = Eto.Forms.Button()
        self.DefaultButton.Text = "OK"
        self.DefaultButton.Click += self.on_ok
        return self.DefaultButton

    @property
    def cancel(self):
        self.AbortButton = Eto.Forms.Button()
        self.AbortButton.Text = "Cancel"
        self.AbortButton.Click += self.on_cancel
        return self.AbortButton

    def is_valid_path(self, value):
        """Check if a string is a valid path-like structure (contains slashes or backslashes), even if the path doesn't exist."""
        if isinstance(value, str):
            # Consider it a path if it contains a forward slash or backslash
            return "\\" in value or "/" in value
        return False


    
    def on_ok(self, sender, event):
        try:
            # Reset the attribute list with the updated values from the table
            self.attributes = []
            for row in self.table.DataStore:
                name = row.GetValue(0)
                value = row.GetValue(1)

                if value != "-" and value != "" and value != " ":  # Ignore empty values
                    try:
                        # If value is a string and looks like a path, keep it as a string
                        if isinstance(value, str) and self.is_valid_path(value):
                            # print(f"Value '{value}' recognized as a valid path format.")
                            pass  # Do nothing, keep value as a string
                        else:
                            # Attempt to evaluate non-path values
                            value = ast.literal_eval(value)
                    except Exception as e:
                        # If evaluation fails, keep it as a string and print an error for debugging
                        print(f"Error evaluating value '{value}': {e}")
                self.attributes.append((name, value))  # Store as tuple
        except Exception as e:
            print(e)
            self.Close(False)
        self.Close(True)

    def on_cancel(self, sender, event):
        self.Close(False)

    def show(self):
        return self.ShowModal(Rhino.UI.RhinoEtoApp.MainWindow)