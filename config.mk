# Compiler and compilation flags
cc=gcc
cxx=g++
cflags=-Wall -ansi -pedantic -O3 -pg -DUSE_CLOCK

# MPI compiler
mpicxx=mpicxx -Wno-long-long

# Flags for linking to PNG library
png_iflags=-DHAS_PNG
png_lflags=-lpng

# LAPACK flags for dense linear algebra
lp_lflags=-llapack -lblas
