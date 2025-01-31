EXE := stoat
SOURCES := src/main.cpp src/pext/pext.cpp

SUFFIX :=

CXX := clang++

CXXFLAGS := -std=c++20 -flto

CXXFLAGS_RELEASE := -O3 -DNDEBUG
CXXFLAGS_SANITIZER := -O1 -g -fsanitize=address -fsanitize=undefined

CXXFLAGS_NATIVE := -DST_NATIVE -march=native

LDFLAGS :=

COMPILER_VERSION := $(shell $(CXX) --version)

ifeq (, $(findstring clang,$(COMPILER_VERSION)))
    ifeq (, $(findstring gcc,$(COMPILER_VERSION)))
        ifeq (, $(findstring g++,$(COMPILER_VERSION)))
            $(error Only Clang and GCC supported)
        endif
    endif
endif

ifeq ($(OS), Windows_NT)
    DETECTED_OS := Windows
    SUFFIX := .exe
else
    DETECTED_OS := $(shell uname -s)
    LDFLAGS += -pthread
endif

ifneq (, $(findstring clang,$(COMPILER_VERSION)))
    ifneq ($(DETECTED_OS),Darwin)
        LDFLAGS += -fuse-ld=lld
    endif
endif

ARCH_DEFINES := $(shell echo | $(CXX) -march=native -E -dM -)

ifneq ($(findstring __BMI2__, $(ARCH_DEFINES)),)
    ifeq ($(findstring __znver1, $(ARCH_DEFINES)),)
        ifeq ($(findstring __znver2, $(ARCH_DEFINES)),)
            ifeq ($(findstring __bdver, $(ARCH_DEFINES)),)
                CXXFLAGS_NATIVE += -DST_FAST_PEXT
            endif
        endif
    endif
endif

define build
    $(CXX) $(CXXFLAGS) $(CXXFLAGS_$1) $(CXXFLAGS_$2) $(LDFLAGS) -o $(EXE)$(if $(NO_EXE_SET),-$3)$(SUFFIX) $^
endef

all: native

.PHONY: all

.DEFAULT_GOAL := native

$(EXE): $(SOURCES)
	$(call build,NATIVE,RELEASE,native)

native: $(EXE)

sanitizer: $(SOURCES)
	$(call build,NATIVE,SANITIZER,native)

clean:

