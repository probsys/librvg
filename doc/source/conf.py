# Configuration file for the Sphinx documentation builder.
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project                 = 'librvg'
copyright               = '2025, Probabilistic Computing Systems Laboratory'
author                  = 'Probabilistic Computing Systems Laboratory'
release                 = '0.1'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions              = ['breathe', 'sphinxcontrib.bibtex', 'sphinx.ext.autosectionlabel']
breathe_projects        = { 'librvg': '../build/dox/xml' }
breathe_default_project = 'librvg'
breathe_show_include    = True
breathe_implementation_filename_extensions = ['.c', '.cc', '.cpp', '.cxx', '.C', '.m', '.mm']


master_doc              = 'index'
primary_domain          = 'c'
highlight_language      = 'c'
pygments_style          = 'sphinx'
numfig                  = True

templates_path          = ['_templates']
exclude_patterns        = []

bibtex_bibfiles         = ['references.bib']
bibtex_default_style    = 'alpha'
bibtex_reference_style  = 'label'

autosectionlabel_prefix_document = True

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output
html_theme              = 'sphinx_rtd_theme'
html_theme_options      = {'navigation_depth': 3}

html_static_path        = ['_static']
html_css_files          = ['custom.css']

