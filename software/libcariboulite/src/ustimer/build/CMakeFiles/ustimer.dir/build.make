# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.18

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/lib/python3.7/dist-packages/cmake/data/bin/cmake

# The command to remove a file.
RM = /usr/local/lib/python3.7/dist-packages/cmake/data/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/pi/projects/cariboulite/software/libcariboulite/src/ustimer

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/pi/projects/cariboulite/software/libcariboulite/src/ustimer/build

# Include any dependencies generated for this target.
include CMakeFiles/ustimer.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/ustimer.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ustimer.dir/flags.make

CMakeFiles/ustimer.dir/ustimer.c.o: CMakeFiles/ustimer.dir/flags.make
CMakeFiles/ustimer.dir/ustimer.c.o: ../ustimer.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/pi/projects/cariboulite/software/libcariboulite/src/ustimer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/ustimer.dir/ustimer.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/ustimer.dir/ustimer.c.o -c /home/pi/projects/cariboulite/software/libcariboulite/src/ustimer/ustimer.c

CMakeFiles/ustimer.dir/ustimer.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/ustimer.dir/ustimer.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/pi/projects/cariboulite/software/libcariboulite/src/ustimer/ustimer.c > CMakeFiles/ustimer.dir/ustimer.c.i

CMakeFiles/ustimer.dir/ustimer.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/ustimer.dir/ustimer.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/pi/projects/cariboulite/software/libcariboulite/src/ustimer/ustimer.c -o CMakeFiles/ustimer.dir/ustimer.c.s

# Object files for target ustimer
ustimer_OBJECTS = \
"CMakeFiles/ustimer.dir/ustimer.c.o"

# External object files for target ustimer
ustimer_EXTERNAL_OBJECTS =

libustimer.a: CMakeFiles/ustimer.dir/ustimer.c.o
libustimer.a: CMakeFiles/ustimer.dir/build.make
libustimer.a: CMakeFiles/ustimer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/pi/projects/cariboulite/software/libcariboulite/src/ustimer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library libustimer.a"
	$(CMAKE_COMMAND) -P CMakeFiles/ustimer.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ustimer.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ustimer.dir/build: libustimer.a

.PHONY : CMakeFiles/ustimer.dir/build

CMakeFiles/ustimer.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/ustimer.dir/cmake_clean.cmake
.PHONY : CMakeFiles/ustimer.dir/clean

CMakeFiles/ustimer.dir/depend:
	cd /home/pi/projects/cariboulite/software/libcariboulite/src/ustimer/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/pi/projects/cariboulite/software/libcariboulite/src/ustimer /home/pi/projects/cariboulite/software/libcariboulite/src/ustimer /home/pi/projects/cariboulite/software/libcariboulite/src/ustimer/build /home/pi/projects/cariboulite/software/libcariboulite/src/ustimer/build /home/pi/projects/cariboulite/software/libcariboulite/src/ustimer/build/CMakeFiles/ustimer.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/ustimer.dir/depend
