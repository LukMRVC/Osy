# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

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
CMAKE_COMMAND = /home/lukas/.local/share/JetBrains/Toolbox/apps/CLion/ch-0/193.6494.38/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/lukas/.local/share/JetBrains/Toolbox/apps/CLion/ch-0/193.6494.38/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/lukas/Documents/osy/proc

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/lukas/Documents/osy/proc/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/proc.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/proc.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/proc.dir/flags.make

CMakeFiles/proc.dir/main.c.o: CMakeFiles/proc.dir/flags.make
CMakeFiles/proc.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lukas/Documents/osy/proc/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/proc.dir/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/proc.dir/main.c.o   -c /home/lukas/Documents/osy/proc/main.c

CMakeFiles/proc.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/proc.dir/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/lukas/Documents/osy/proc/main.c > CMakeFiles/proc.dir/main.c.i

CMakeFiles/proc.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/proc.dir/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/lukas/Documents/osy/proc/main.c -o CMakeFiles/proc.dir/main.c.s

# Object files for target proc
proc_OBJECTS = \
"CMakeFiles/proc.dir/main.c.o"

# External object files for target proc
proc_EXTERNAL_OBJECTS =

proc: CMakeFiles/proc.dir/main.c.o
proc: CMakeFiles/proc.dir/build.make
proc: CMakeFiles/proc.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/lukas/Documents/osy/proc/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable proc"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/proc.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/proc.dir/build: proc

.PHONY : CMakeFiles/proc.dir/build

CMakeFiles/proc.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/proc.dir/cmake_clean.cmake
.PHONY : CMakeFiles/proc.dir/clean

CMakeFiles/proc.dir/depend:
	cd /home/lukas/Documents/osy/proc/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lukas/Documents/osy/proc /home/lukas/Documents/osy/proc /home/lukas/Documents/osy/proc/cmake-build-debug /home/lukas/Documents/osy/proc/cmake-build-debug /home/lukas/Documents/osy/proc/cmake-build-debug/CMakeFiles/proc.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/proc.dir/depend
