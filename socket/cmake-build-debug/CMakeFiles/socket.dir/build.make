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
CMAKE_SOURCE_DIR = /home/lukas/Documents/osy/socket

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/lukas/Documents/osy/socket/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/socket.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/socket.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/socket.dir/flags.make

CMakeFiles/socket.dir/socket_cl.cpp.o: CMakeFiles/socket.dir/flags.make
CMakeFiles/socket.dir/socket_cl.cpp.o: ../socket_cl.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lukas/Documents/osy/socket/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/socket.dir/socket_cl.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/socket.dir/socket_cl.cpp.o -c /home/lukas/Documents/osy/socket/socket_cl.cpp

CMakeFiles/socket.dir/socket_cl.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/socket.dir/socket_cl.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/lukas/Documents/osy/socket/socket_cl.cpp > CMakeFiles/socket.dir/socket_cl.cpp.i

CMakeFiles/socket.dir/socket_cl.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/socket.dir/socket_cl.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/lukas/Documents/osy/socket/socket_cl.cpp -o CMakeFiles/socket.dir/socket_cl.cpp.s

# Object files for target socket
socket_OBJECTS = \
"CMakeFiles/socket.dir/socket_cl.cpp.o"

# External object files for target socket
socket_EXTERNAL_OBJECTS =

socket: CMakeFiles/socket.dir/socket_cl.cpp.o
socket: CMakeFiles/socket.dir/build.make
socket: CMakeFiles/socket.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/lukas/Documents/osy/socket/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable socket"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/socket.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/socket.dir/build: socket

.PHONY : CMakeFiles/socket.dir/build

CMakeFiles/socket.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/socket.dir/cmake_clean.cmake
.PHONY : CMakeFiles/socket.dir/clean

CMakeFiles/socket.dir/depend:
	cd /home/lukas/Documents/osy/socket/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lukas/Documents/osy/socket /home/lukas/Documents/osy/socket /home/lukas/Documents/osy/socket/cmake-build-debug /home/lukas/Documents/osy/socket/cmake-build-debug /home/lukas/Documents/osy/socket/cmake-build-debug/CMakeFiles/socket.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/socket.dir/depend

