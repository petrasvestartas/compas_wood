# flake8: noqa
# -*- coding: utf-8 -*-

from sphinx.writers import html, html5
import sphinx_compas2_theme

# import os
# import sys
# sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '../src')))
# sys.path.insert(0, '/home/petras/brg/2_code/wood_nano/src')
# import compas_wood
# import wood_nano
# import compas_wood.binding


# -- General configuration ------------------------------------------------

project = "compas_wood"
copyright = "petras vestartas"
author = "petras vestartas"
package = "compas_wood"
organization = "petrasvestartas"

master_doc = "index"
source_suffix = {".rst": "restructuredtext", ".md": "markdown"}
templates_path = sphinx_compas2_theme.get_autosummary_templates_path()
exclude_patterns = sphinx_compas2_theme.default_exclude_patterns
add_module_names = True
language = "en"

latest_version = sphinx_compas2_theme.get_latest_version()

if latest_version == "Unreleased":
    release = "Unreleased"
    version = "latest"
else:
    release = latest_version
    version = ".".join(release.split(".")[0:2])  # type: ignore

# -- Extension configuration ------------------------------------------------

extensions = sphinx_compas2_theme.default_extensions
extensions.remove("sphinx.ext.linkcode")

# numpydoc options

numpydoc_show_class_members = False
numpydoc_class_members_toctree = False
numpydoc_attributes_as_param_list = True

# bibtex options

# autodoc options

autodoc_type_aliases = {}

autodoc_typehints = "description"
autodoc_typehints_format = "short"
autodoc_typehints_description_target = "documented"

autodoc_mock_imports = sphinx_compas2_theme.default_mock_imports
autodoc_mock_imports = ['wood_nano']

autodoc_default_options = {
    "undoc-members": True,
    "show-inheritance": True,
}

autodoc_member_order = "groupwise"


autoclass_content = "class"


def setup(app):
    app.connect("autodoc-skip-member", sphinx_compas2_theme.skip)


# autosummary options

autosummary_generate = True
autosummary_mock_imports = sphinx_compas2_theme.default_mock_imports


# graph options

# plot options

# intersphinx options

intersphinx_mapping = {
    "python": ("https://docs.python.org/", None),
    "compas": ("https://compas.dev/compas/latest/", None),
}

# linkcode

linkcode_resolve = sphinx_compas2_theme.get_linkcode_resolve(organization, package)

# extlinks

extlinks = {}

# from pytorch

sphinx_compas2_theme.replace(html.HTMLTranslator)
sphinx_compas2_theme.replace(html5.HTML5Translator)

# -- Options for HTML output ----------------------------------------------

html_theme = "sidebaronly"
html_title = project

favicons = [
    {
        "rel": "icon",
        "href": "compas.ico",
    }
]

html_theme_options = {
    "icon_links": [
        {
            "name": "GitHub",
            "url": f"https://github.com/{organization}/{package}",
            "icon": "fa-brands fa-github",
            "type": "fontawesome",
        },
        {
            "name": "Discourse",
            "url": "http://forum.compas-framework.org/",
            "icon": "fa-brands fa-discourse",
            "type": "fontawesome",
        },
        {
            "name": "PyPI",
            "url": f"https://pypi.org/project/{package}/",
            "icon": "fa-brands fa-python",
            "type": "fontawesome",
        },
    ],
    "switcher": {
        "json_url": f"https://raw.githubusercontent.com/{organization}/{package}/gh-pages/versions.json",
        "version_match": version,
    },
    "check_switcher": False,
    "logo": {
        "image_light": "_static/compas_icon_white.png",
        "image_dark": "_static/compas_icon_white.png",
        "text": project,
    },
    "navigation_depth": 3,
}


html_context = {
    "github_url": "https://github.com",
    "github_user": organization,
    "github_repo": package,
    "github_version": "main",
    "doc_path": "docs",
}

html_static_path = sphinx_compas2_theme.get_html_static_path() + ["_static"]
html_css_files = []
html_extra_path = []
html_last_updated_fmt = ""
html_copy_source = False
html_show_sourcelink = True
html_permalinks = False
html_permalinks_icon = ""
html_compact_lists = True
