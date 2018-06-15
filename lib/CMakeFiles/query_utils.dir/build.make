# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /global/homes/w/wzhang5/software/HDF5Meta

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /global/homes/w/wzhang5/software/HDF5Meta

# Include any dependencies generated for this target.
include lib/CMakeFiles/query_utils.dir/depend.make

# Include the progress variables for this target.
include lib/CMakeFiles/query_utils.dir/progress.make

# Include the compile flags for this target's objects.
include lib/CMakeFiles/query_utils.dir/flags.make

lib/CMakeFiles/query_utils.dir/query_utils.c.o: lib/CMakeFiles/query_utils.dir/flags.make
lib/CMakeFiles/query_utils.dir/query_utils.c.o: lib/query_utils.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/global/homes/w/wzhang5/software/HDF5Meta/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object lib/CMakeFiles/query_utils.dir/query_utils.c.o"
	cd /global/homes/w/wzhang5/software/HDF5Meta/lib && /opt/cray/pe/craype/2.5.12/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/query_utils.dir/query_utils.c.o   -c /global/homes/w/wzhang5/software/HDF5Meta/lib/query_utils.c

lib/CMakeFiles/query_utils.dir/query_utils.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/query_utils.dir/query_utils.c.i"
	cd /global/homes/w/wzhang5/software/HDF5Meta/lib && /opt/cray/pe/craype/2.5.12/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /global/homes/w/wzhang5/software/HDF5Meta/lib/query_utils.c > CMakeFiles/query_utils.dir/query_utils.c.i

lib/CMakeFiles/query_utils.dir/query_utils.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/query_utils.dir/query_utils.c.s"
	cd /global/homes/w/wzhang5/software/HDF5Meta/lib && /opt/cray/pe/craype/2.5.12/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /global/homes/w/wzhang5/software/HDF5Meta/lib/query_utils.c -o CMakeFiles/query_utils.dir/query_utils.c.s

lib/CMakeFiles/query_utils.dir/query_utils.c.o.requires:

.PHONY : lib/CMakeFiles/query_utils.dir/query_utils.c.o.requires

lib/CMakeFiles/query_utils.dir/query_utils.c.o.provides: lib/CMakeFiles/query_utils.dir/query_utils.c.o.requires
	$(MAKE) -f lib/CMakeFiles/query_utils.dir/build.make lib/CMakeFiles/query_utils.dir/query_utils.c.o.provides.build
.PHONY : lib/CMakeFiles/query_utils.dir/query_utils.c.o.provides

lib/CMakeFiles/query_utils.dir/query_utils.c.o.provides.build: lib/CMakeFiles/query_utils.dir/query_utils.c.o


# Object files for target query_utils
query_utils_OBJECTS = \
"CMakeFiles/query_utils.dir/query_utils.c.o"

# External object files for target query_utils
query_utils_EXTERNAL_OBJECTS =

lib/libquery_utils.a: lib/CMakeFiles/query_utils.dir/query_utils.c.o
lib/libquery_utils.a: lib/CMakeFiles/query_utils.dir/build.make
lib/libquery_utils.a: lib/CMakeFiles/query_utils.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/global/homes/w/wzhang5/software/HDF5Meta/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library libquery_utils.a"
	cd /global/homes/w/wzhang5/software/HDF5Meta/lib && $(CMAKE_COMMAND) -P CMakeFiles/query_utils.dir/cmake_clean_target.cmake
	cd /global/homes/w/wzhang5/software/HDF5Meta/lib && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/query_utils.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
lib/CMakeFiles/query_utils.dir/build: lib/libquery_utils.a

.PHONY : lib/CMakeFiles/query_utils.dir/build

lib/CMakeFiles/query_utils.dir/requires: lib/CMakeFiles/query_utils.dir/query_utils.c.o.requires

.PHONY : lib/CMakeFiles/query_utils.dir/requires

lib/CMakeFiles/query_utils.dir/clean:
	cd /global/homes/w/wzhang5/software/HDF5Meta/lib && $(CMAKE_COMMAND) -P CMakeFiles/query_utils.dir/cmake_clean.cmake
.PHONY : lib/CMakeFiles/query_utils.dir/clean

lib/CMakeFiles/query_utils.dir/depend:
	cd /global/homes/w/wzhang5/software/HDF5Meta && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /global/homes/w/wzhang5/software/HDF5Meta /global/homes/w/wzhang5/software/HDF5Meta/lib /global/homes/w/wzhang5/software/HDF5Meta /global/homes/w/wzhang5/software/HDF5Meta/lib /global/homes/w/wzhang5/software/HDF5Meta/lib/CMakeFiles/query_utils.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : lib/CMakeFiles/query_utils.dir/depend

