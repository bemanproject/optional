#! /usr/bin/make -f
# Makefile                                                       -*-makefile-*-
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

BASH := $(shell command -v bash 2>/dev/null)
ifeq ($(BASH),)
  $(error bash not found; install bash or add it to PATH)
endif
SHELL := $(BASH)

INSTALL_PREFIX?=.install/
BUILD_DIR?=.build
DEST?=$(INSTALL_PREFIX)
CMAKE_FLAGS?=

TARGETS := test clean all ctest

export

.update-submodules:
	git submodule update --init --recursive
	touch .update-submodules

.gitmodules: .update-submodules

CONFIG?=Asan

export

ifeq ($(strip $(TOOLCHAIN)),)
	_build_name?=build-system/
	_build_dir?=.build/
	_configuration_types?="RelWithDebInfo;Debug;Tsan;Asan;Gcov"
	_cmake_args=-DCMAKE_TOOLCHAIN_FILE=$(CURDIR)/etc/toolchain.cmake
else
	_build_name?=build-$(TOOLCHAIN)
	_build_dir?=.build/
	_configuration_types?="RelWithDebInfo;Debug;Tsan;Asan;Gcov"
	_cmake_args=-DCMAKE_TOOLCHAIN_FILE=$(CURDIR)/etc/$(TOOLCHAIN)-toolchain.cmake
endif


_build_path?=$(_build_dir)/$(_build_name)
_build_path:=$(subst //,/,$(_build_path))
_build_path:=$(patsubst %/,%,$(_build_path))

define run_cmake =
	cmake \
	-G "Ninja Multi-Config" \
	-DCMAKE_CONFIGURATION_TYPES=$(_configuration_types) \
	-DCMAKE_INSTALL_PREFIX=$(abspath $(INSTALL_PREFIX)) \
	-DCMAKE_EXPORT_COMPILE_COMMANDS=1 \
	-DCMAKE_PREFIX_PATH=$(CURDIR)/infra/cmake \
    -DCMAKE_PROJECT_TOP_LEVEL_INCLUDES="./cmake/use-fetch-content.cmake;infra/cmake/BuildTelemetry.cmake" \
	$(_cmake_args) \
	$(CURDIR)
endef

default: test

$(_build_path):
	mkdir -p $(_build_path)

$(_build_path)/CMakeCache.txt: | $(_build_path) .gitmodules
	cd $(_build_path) && $(run_cmake)

$(_build_path)/compile_commands.json : $(_build_path)/CMakeCache.txt

.PHONY: compile_commands.json
compile_commands.json:
	if [ "$(shell readlink compile_commands.json)" != "$(_build_path)/compile_commands.json" ] ; then \
		ln -sf $(_build_path)/compile_commands.json ; \
	fi

TARGET:=all
compile: $(_build_path)/CMakeCache.txt
compile: compile_commands.json
compile:  ## Compile the project
	cmake --build $(_build_path)  --config $(CONFIG) --target all -- -k 0

compile-headers: $(_build_path)/CMakeCache.txt ## Compile the headers
	cmake --build $(_build_path)  --config $(CONFIG) --target all_verify_interface_header_sets -- -k 0

install: $(_build_path)/CMakeCache.txt compile ## Install the project
	cmake --install $(_build_path) --config $(CONFIG) --component optional_Development --verbose

.PHONY: clean-install
clean-install:
	-rm -rf .install

realclean: clean-install

ctest: $(_build_path)/CMakeCache.txt ## Run CTest on current build
	cd $(_build_path) && ctest --parallel --output-on-failure -C $(CONFIG)

ctest_ : compile
	cd $(_build_path) && ctest --parallel --output-on-failure -C $(CONFIG)

test: ctest_ ## Rebuild and run tests

cmake: |  $(_build_path)
	cd $(_build_path) && ${run_cmake}

clean: $(_build_path)/CMakeCache.txt ## Clean the build artifacts
	cmake --build $(_build_path)  --config $(CONFIG) --target clean

realclean: ## Delete the build directory
	rm -rf $(_build_path)

env:
	$(foreach v, $(.VARIABLES), $(info $(v) = $($(v))))

.PHONY : compile install ctest ctest_ test cmake clean realclean env

.PHONY: papers
papers:
	$(MAKE) -C papers/P2988 papers

.DEFAULT: $(_build_path)/CMakeCache.txt ## Other targets passed through to cmake
	cmake --build $(_build_path)  --config $(CONFIG) --target $@ -- -k 0

PYEXECPATH ?= $(shell which python3.13 || which python3.12 || which python3.11 || which python3.10 || which python3.9 || which python3.8 || which python3)
PYTHON ?= $(notdir $(PYEXECPATH))
VENV := .venv
ACTIVATE := . $(VENV)/bin/activate &&
PYEXEC := $(ACTIVATE) $(PYTHON)
MARKER=.initialized.venv.stamp

PIP := $(PYEXEC) -m pip
PIP_SYNC := $(PYEXEC) -m piptools sync
PIPTOOLS_COMPILE := $(PYEXEC) -m piptools compile --no-header --strip-extras

PRE_COMMIT := $(ACTIVATE) pre-commit

PHONY: venv
venv: ## Create python virtual env
venv: $(VENV)/$(MARKER)

.PHONY: clean-venv
clean-venv:
clean-venv: ## Delete python virtual env
	-rm -rf $(VENV)

realclean: clean-venv

.PHONY: show-venv
show-venv: venv
show-venv: ## Debugging target - show venv details
	$(PYEXEC) -c "import sys; print('Python ' + sys.version.replace('\n',''))"
	$(PIP) --version
	@echo venv: $(VENV)

requirements.txt: requirements.in
	$(PIPTOOLS_COMPILE) --output-file=$@ $<

requirements-dev.txt: requirements-dev.in
	$(PIPTOOLS_COMPILE) --output-file=$@ $<

$(VENV):
	$(PYEXECPATH) -m venv $(VENV)
	$(PIP) install pip setuptools wheel
	$(PIP) install pip-tools

$(VENV)/$(MARKER): requirements.txt requirements-dev.txt | $(VENV)
	$(PIP_SYNC) requirements.txt
	$(PIP_SYNC) requirements-dev.txt
	touch $(VENV)/$(MARKER)

.PHONY: dev-shell
dev-shell: venv
dev-shell: ## Shell with the venv activated
	$(ACTIVATE) $(notdir $(SHELL))

.PHONY: bash zsh
bash zsh: venv
bash zsh: ## Run bash or zsh with the venv activated
	$(ACTIVATE) exec $@

.PHONY: lint
lint: venv
lint: ## Run all configured tools in pre-commit
	$(PRE_COMMIT) run -a

.PHONY: lint-manual
lint-manual: venv
lint-manual: ## Run all manual tools in pre-commit
	$(PRE_COMMIT) run --hook-stage manual -a

.PHONY: coverage
coverage: ## Build and run the tests with the GCOV profile and process the results
coverage: venv $(_build_path)/CMakeCache.txt
	$(ACTIVATE) cmake --build $(_build_path) --config Gcov
	$(ACTIVATE) ctest --build-config Gcov --output-on-failure --test-dir $(_build_path)
	$(ACTIVATE) cmake --build $(_build_path) --config Gcov --target process_coverage

.PHONY: view-coverage
view-coverage: ## View the coverage report
	sensible-browser $(_build_path)/coverage/coverage.html

# Documentation tools
# If a versioned clang toolchain is active (TOOLCHAIN=clang-N), use that
# exact compiler for MrDocs so the docs build matches the compile environment.
# For gcc, unversioned, or no toolchain, auto-detect the newest available clang++.
ifneq ($(filter clang-%,$(TOOLCHAIN)),)
  _docs_cxx := $(shell command -v "clang++$(patsubst clang%,%,$(TOOLCHAIN))" 2>/dev/null)
endif
ifeq ($(_docs_cxx),)
  _docs_cxx := $(shell for c in clang++-21 clang++-20 clang++-19 clang++-18 clang++; do command -v "$$c" 2>/dev/null && break; done)
endif

MRDOCS_VERSION ?= latest
MRDOCS_INSTALL_DIR ?= .tools/mrdocs
MRDOCS ?= $(MRDOCS_INSTALL_DIR)/bin/mrdocs

_uname_s := $(shell uname -s)

ifeq ($(_uname_s),Linux)
  _mrdocs_os := Linux
else ifeq ($(_uname_s),Darwin)
  _mrdocs_os := Darwin
endif

$(MRDOCS):
	etc/install-mrdocs.sh \
	    --version $(MRDOCS_VERSION) \
	    --install-dir $(MRDOCS_INSTALL_DIR) \
	    --os $(_mrdocs_os)

.PHONY: install-mrdocs
install-mrdocs: $(MRDOCS) ## Install MrDocs locally

.PHONY: update-mrdocs
update-mrdocs: ## Update MrDocs (use MRDOCS_VERSION=vX.Y.Z to pin)
	rm -rf $(MRDOCS_INSTALL_DIR)
	$(MAKE) install-mrdocs

node_modules/.package-lock.json: package.json package-lock.json
	npm ci

.PHONY: install-antora
install-antora: node_modules/.package-lock.json ## Install Antora and extensions via npm

.PHONY: update-antora
update-antora: ## Update Antora npm dependencies
	npm update

.PHONY: install-tools
install-tools: install-mrdocs install-antora ## Install all documentation tools (MrDocs, Antora)

.PHONY: update-tools
update-tools: update-mrdocs update-antora ## Update all documentation tools to latest

.PHONY: clean-tools
clean-tools: ## Remove locally installed documentation tools
	-rm -rf .tools node_modules

realclean: clean-tools clean-docs

_docs_conf  := antora-playbook.yml antora/antora-worktree-fix.js docs/antora.yml docs/mrdocs.yml

# Docs output lives under the toolchain build path so it uses the same
# compilation environment as the rest of the build, and the root-level
# compile_commands.json symlink (which may point to a different toolchain)
# is never consulted.  --to-dir overrides antora-playbook.yml's output.dir,
# which retains a sensible default for standalone `npx antora` invocations.
DOCS_OUT   := $(_build_path)/site
DOCS_STAMP := $(DOCS_OUT)/.docs.stamp
DOCS_DEPS  := $(DOCS_OUT)/.docs.d

-include $(DOCS_DEPS)
# Explicit empty rule so -include does not fall through to .DEFAULT when the
# dep file is absent (first build or after clean-docs).
$(DOCS_DEPS): ;
# Same protection for the docs source files: if make cannot find them as
# ordinary files (e.g. no cmake build dir exists in a docs-only CI run) it
# must not fall through to the .DEFAULT cmake-passthrough rule.
$(_docs_conf): ;

$(DOCS_STAMP): $(_docs_conf) node_modules/.package-lock.json $(MRDOCS)
	CXX=$(_docs_cxx) MRDOCS_ROOT=$(abspath $(MRDOCS_INSTALL_DIR)) \
	    npx antora --to-dir $(abspath $(DOCS_OUT)) antora-playbook.yml
	@{ find include -name '*.hpp'; find docs/modules -name '*.adoc'; } \
	    | awk -v s="$@" '{ print s ": " $$0; print $$0 ":" }' > $(DOCS_DEPS)
	@touch $(DOCS_STAMP)

.PHONY: docs
docs: install-antora install-mrdocs $(DOCS_STAMP) ## Build documentation site with Antora + MrDocs

.PHONY: print-docs-out
print-docs-out: ## Print the docs output directory (used by CI to locate the built site)
	@echo $(abspath $(DOCS_OUT))

.PHONY: clean-docs
clean-docs: ## Remove generated Antora site
	-rm -rf $(DOCS_OUT)

clean: clean-docs

.PHONY: view-docs
view-docs: $(DOCS_STAMP) ## Open the built documentation site in a browser
	sensible-browser $(DOCS_OUT)/index.html

.PHONY: mrdocs
mrdocs: $(_docs_conf) node_modules/.package-lock.json $(MRDOCS) ## Generate API reference pages with MrDocs (without full Antora build)
	cd docs && CXX=$(_docs_cxx) NO_COLOR=1 $(abspath $(MRDOCS)) mrdocs.yml 2>&1 | sed 's/\x1b\[[0-9;]*m//g'

.PHONY: clean-mrdocs
clean-mrdocs: ## Remove generated MrDocs reference pages
	-rm -rf docs/modules/ROOT/pages/reference

clean: clean-mrdocs

.PHONY: testinstall
testinstall: install
testinstall: ## Test the installed package
	cmake -S installtest -B installtest/.build
	cmake --build  installtest/.build --target test

.PHONY: clean-testinstall
clean-testinstall:
	-rm -rf installtest/.build

realclean: clean-testinstall

# Help target
.PHONY: help
help: ## Show this help.
	@awk 'BEGIN {FS = ":.*?## "} /^[.a-zA-Z_-]+:.*?## / {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'  $(MAKEFILE_LIST) | sort
