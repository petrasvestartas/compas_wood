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

    Attributes
    ----------
    attributes : list[tuple[str, str]]
        The list of names and values as tuples.

    Returns
    -------
    bool
        True if the user clicked OK, False if the user clicked Cancel.

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
                            pass  # Do nothing, keep value as a string
                        else:
                            # Attempt to evaluate non-path values
                            value = ast.literal_eval(value)
                            # Check if the evaluated value is a list of lists
                            if isinstance(value, list) and all(isinstance(i, list) for i in value):
                                print("List of lists detected", type(value), value)
                            else:
                                print("Not a list of lists", type(value), value)
                    except (ValueError, SyntaxError):  # Catch specific literal eval errors
                        # If eval fails, we keep it as a string
                        pass
                self.attributes.append((name, value))  # Store as tuple
        except Exception as e:
            print(e)
            self.Close(False)
        self.Close(True)

    def on_cancel(self, sender, event):
        self.Close(False)

    def show(self):
        return self.ShowModal(Rhino.UI.RhinoEtoApp.MainWindow)


class BooleanForm(Eto.Forms.Dialog[bool]):
    """Eto form for displaying and editing named values using boolean toggles.

    Parameters
    ----------
    names : list[str]
        The names of the values.
    values: list[bool]
        The values as booleans.
    title : str, optional
        The title of the form window.
    width : int, optional
        The width of the form window.
    height : int, optional
        The height of the form window.

    Attributes
    ----------
    attributes : list[tuple[str, bool]]
        The list of names and boolean values as tuples.

    Returns
    -------
    bool
        True if the user clicked OK, False if the user clicked Cancel.
    """

    def __init__(
        self,
        names,  # type: list[str]
        values,  # type: list[bool]
        title="Boolean Form",  # type: str
        width=600,  # type: int
        height=800,  # type: int
    ):
        # type: (...) -> None
        super().__init__()

        # Store as a list of tuples (allowing duplicates)
        self.attributes = list(zip(names, values))
        self.names = names
        self.values = values

        self.Title = title
        self.Padding = Eto.Drawing.Padding(0)
        self.Resizable = True
        self.MinimumSize = Eto.Drawing.Size(0.5 * width, 0.5 * height)
        self.ClientSize = Eto.Drawing.Size(width, height)

        # Create the table (GridView)
        self.table = table = Eto.Forms.GridView()
        table.ShowHeader = True

        # First column: Names (not editable)
        name_column = Eto.Forms.GridColumn()
        name_column.HeaderText = "Name"
        name_column.Editable = False
        name_column.DataCell = Eto.Forms.TextBoxCell(0)  # Display names as text
        table.Columns.Add(name_column)

        # Second column: Values as boolean toggle (editable)
        value_column = Eto.Forms.GridColumn()
        value_column.HeaderText = "Value"
        value_column.Editable = True
        value_column.DataCell = Eto.Forms.CheckBoxCell(1)  # Display values as checkbox
        table.Columns.Add(value_column)

        # Populate the table with names and boolean values
        collection = []
        for name, value in zip(self.names, self.values):
            item = Eto.Forms.GridItem()
            item.Values = (name, value)  # Set name and boolean value
            collection.append(item)
        table.DataStore = collection

        # Layout for the form
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

    def on_ok(self, sender, event):
        try:
            # Reset the attribute list with updated values from the table
            self.attributes = []
            for row in self.table.DataStore:
                name = row.GetValue(0)
                value = row.GetValue(1)

                # Store name and boolean value as tuple
                self.attributes.append((name, bool(value)))
        except Exception as e:
            print(e)
            self.Close(False)
        self.Close(True)

    def on_cancel(self, sender, event):
        self.Close(False)

    def show(self):
        return self.ShowModal(Rhino.UI.RhinoEtoApp.MainWindow)
