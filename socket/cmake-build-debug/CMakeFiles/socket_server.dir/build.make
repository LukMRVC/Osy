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
CMAKE_COMMAND = /home/lukas/.local/share/JetBrains/Toolbox/apps/CLion/ch-0/193.6911.21/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/lukas/.local/share/JetBrains/Toolbox/apps/CLion/ch-0/193.6911.21/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/lukas/Documents/osy/socket

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/lukas/Documents/osy/socket/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/socket_server.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/socket_server.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/socket_server.dir/flags.make

CMakeFiles/socket_server.dir/socket_srv.cpp.o: CMakeFiles/socket_server.dir/flags.make
CMakeFiles/socket_server.dir/socket_srv.cpp.o: ../socket_srv.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lukas/Documents/osy/socket/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/socket_server.dir/socket_srv.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/socket_server.dir/socket_srv.cpp.o -c /home/lukas/Documents/osy/socket/socket_srv.cpp

CMakeFiles/socket_server.dir/socket_srv.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/socket_server.dir/socket_srv.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/lukas/Documents/osy/socket/socket_srv.cpp > CMakeFiles/socket_server.dir/socket_srv.cpp.i

CMakeFiles/socket_server.dir/socket_srv.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/socket_server.dir/socket_srv.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/lukas/Documents/osy/socket/socket_srv.cpp -o CMakeFiles/socket_server.dir/socket_srv.cpp.s

# Object files for target socket_server
socket_server_OBJECTS = \
"CMakeFiles/socket_server.dir/socket_srv.cpp.o"

# External object files for target socket_server
socket_server_EXTERNAL_OBJECTS =

socket_server: CMakeFiles/socket_server.dir/socket_srv.cpp.o
socket_server: CMakeFiles/socket_server.dir/build.make
socket_server: CMakeFiles/socket_server.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/lukas/Documents/osy/socket/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable socket_server"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/socket_server.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/socket_server.dir/build: socket_server

.PHONY : CMakeFiles/socket_server.dir/build

CMakeFiles/socket_server.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/socket_server.dir/cmake_clean.cmake
.PHONY : CMakeFiles/socket_server.dir/clean

CMakeFiles/socket_server.dir/depend:
	cd /home/lukas/Documents/osy/socket/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lukas/Documents/osy/socket /home/lukas/Documents/osy/socket /home/lukas/Documents/osy/socket/cmake-build-debug /home/lukas/Documents/osy/socket/cmake-build-debug /home/lukas/Documents/osy/socket/cmake-build-debug/CMakeFiles/socket_server.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/socket_server.dir/depend

