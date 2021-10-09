.PHONY: doc doc-html doc-latex doc-pdf

DOC_SRC_DIR := doc
DOC_BUILD_DIR := $(BUILD_DIR)/doc
DOC_SPHINX_CMD ?= sphinx-build
DOC_SPHINX_OPTS ?=

doc: doc-html doc-pdf

doc-html:
	@$(DOC_SPHINX_CMD) -M html $(DOC_SRC_DIR) $(DOC_BUILD_DIR) \
		$(DOC_SPHINX_OPTS)

doc-latex:
	@$(DOC_SPHINX_CMD) -M latex $(DOC_SRC_DIR) $(DOC_BUILD_DIR) \
		$(DOC_SPHINX_OPTS)

doc-pdf:
	@$(DOC_SPHINX_CMD) -M latexpdf $(DOC_SRC_DIR) $(DOC_BUILD_DIR) \
		$(DOC_SPHINX_OPTS)
