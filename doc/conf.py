#######################################################################
# Configuration file for the Sphinx documentation builder.
#######################################################################

# -- Project information -----------------------------------------------------

project = 'e4'
copyright = '2021, Shawn M. Chapla'
author = 'Shawn M. Chapla'
release = '0.2.1-dev'


# -- Additional Sphinx Configuration -----------------------------------------

source_suffix = {
    '.rst': 'restructuredtext',
    '.txt': 'restructuredtext',
}


# -- Options for HTML output -------------------------------------------------

html_theme = 'bizstyle'
html_static_path = ['']

# FIXME: Add logo.
# html_logo = 'path/to/logo'
# FIXME: Uncomment the following assignment if needed for logo.
# Otherwise, remove it.
# html_static_path = ['sphinx-static']


# -- Options for LaTeX output ------------------------------------------------

latex_engine = 'pdflatex'
latex_show_pagerefs = True
latex_show_urls = 'footnote'

# FIXME: Add logo.
# latex_logo = 'path/to/logo'
