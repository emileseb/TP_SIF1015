# This is the only thing to change in this Makefile
# >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#Nom de l'étudiant: Emile SEBASTIANUTTI
#Code permanent de l'étudiant:SEBE03079800
# >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

SHELL := /bin/bash

# C++ compiler
CC=g++
NDF=
RDF=

ifdef REDUCED_DEBUG
RDF= -D REDUCED_DEBUG
endif
ifdef NO_DEBUG
NDF= -D NO_DEBUG
endif
CCFLAGS=-c -Os -s -fPIC -std=gnu++11 -D LINUX $(RDF) $(NDF) -lpthread

INC=-I./hdr

# Archiver for .a library
AR=ar
ARFLAGS=-rv

# Ranlib for .a library
RANLIB=ranlib
RANLIBFLAGS=

# Flags for linking .so library
LDFLAGS=

# Source files
SOURCES=$(shell find ./src/ -name "*.cpp" | sed s:./src/:: | tr "\n" " ")
OBJECTS=$(patsubst %.cpp,build/%.o,$(SOURCES))

# Main build targets
TARGET_BASE=ftp_serv
TARGET_A=bin/$(TARGET_BASE)

# Default target - build everything
all: static
static: precomp $(TARGET_A)

$(TARGET_A): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET_A) -lpthread
	
# Compile sources 
build/%.o: ./src/%.cpp
	$(CC) $(CCFLAGS) $(INC) $< -o $@

clean: 
	rm -rf build/*
	rm -rf bin/*

# Ensure required sub-directories are created
precomp: clean
	@mkdir -p build
	@mkdir -p bin