# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_SOURCE_DIR = /home/encheng/code_ui/ui/ui/src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/encheng/code_ui/ui/ui/BUILD

# Include any dependencies generated for this target.
include arbe_gui/os/CMakeFiles/os.dir/depend.make

# Include the progress variables for this target.
include arbe_gui/os/CMakeFiles/os.dir/progress.make

# Include the compile flags for this target's objects.
include arbe_gui/os/CMakeFiles/os.dir/flags.make

arbe_gui/os/CMakeFiles/os.dir/os_autogen/mocs_compilation.cpp.o: arbe_gui/os/CMakeFiles/os.dir/flags.make
arbe_gui/os/CMakeFiles/os.dir/os_autogen/mocs_compilation.cpp.o: arbe_gui/os/os_autogen/mocs_compilation.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/encheng/code_ui/ui/ui/BUILD/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object arbe_gui/os/CMakeFiles/os.dir/os_autogen/mocs_compilation.cpp.o"
	cd /home/encheng/code_ui/ui/ui/BUILD/arbe_gui/os && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/os.dir/os_autogen/mocs_compilation.cpp.o -c /home/encheng/code_ui/ui/ui/BUILD/arbe_gui/os/os_autogen/mocs_compilation.cpp

arbe_gui/os/CMakeFiles/os.dir/os_autogen/mocs_compilation.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/os.dir/os_autogen/mocs_compilation.cpp.i"
	cd /home/encheng/code_ui/ui/ui/BUILD/arbe_gui/os && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/encheng/code_ui/ui/ui/BUILD/arbe_gui/os/os_autogen/mocs_compilation.cpp > CMakeFiles/os.dir/os_autogen/mocs_compilation.cpp.i

arbe_gui/os/CMakeFiles/os.dir/os_autogen/mocs_compilation.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/os.dir/os_autogen/mocs_compilation.cpp.s"
	cd /home/encheng/code_ui/ui/ui/BUILD/arbe_gui/os && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/encheng/code_ui/ui/ui/BUILD/arbe_gui/os/os_autogen/mocs_compilation.cpp -o CMakeFiles/os.dir/os_autogen/mocs_compilation.cpp.s

arbe_gui/os/CMakeFiles/os.dir/vx_event.c.o: arbe_gui/os/CMakeFiles/os.dir/flags.make
arbe_gui/os/CMakeFiles/os.dir/vx_event.c.o: /home/encheng/code_ui/ui/ui/src/arbe_gui/os/vx_event.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/encheng/code_ui/ui/ui/BUILD/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object arbe_gui/os/CMakeFiles/os.dir/vx_event.c.o"
	cd /home/encheng/code_ui/ui/ui/BUILD/arbe_gui/os && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/os.dir/vx_event.c.o   -c /home/encheng/code_ui/ui/ui/src/arbe_gui/os/vx_event.c

arbe_gui/os/CMakeFiles/os.dir/vx_event.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/os.dir/vx_event.c.i"
	cd /home/encheng/code_ui/ui/ui/BUILD/arbe_gui/os && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/encheng/code_ui/ui/ui/src/arbe_gui/os/vx_event.c > CMakeFiles/os.dir/vx_event.c.i

arbe_gui/os/CMakeFiles/os.dir/vx_event.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/os.dir/vx_event.c.s"
	cd /home/encheng/code_ui/ui/ui/BUILD/arbe_gui/os && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/encheng/code_ui/ui/ui/src/arbe_gui/os/vx_event.c -o CMakeFiles/os.dir/vx_event.c.s

arbe_gui/os/CMakeFiles/os.dir/vx_mutex.c.o: arbe_gui/os/CMakeFiles/os.dir/flags.make
arbe_gui/os/CMakeFiles/os.dir/vx_mutex.c.o: /home/encheng/code_ui/ui/ui/src/arbe_gui/os/vx_mutex.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/encheng/code_ui/ui/ui/BUILD/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object arbe_gui/os/CMakeFiles/os.dir/vx_mutex.c.o"
	cd /home/encheng/code_ui/ui/ui/BUILD/arbe_gui/os && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/os.dir/vx_mutex.c.o   -c /home/encheng/code_ui/ui/ui/src/arbe_gui/os/vx_mutex.c

arbe_gui/os/CMakeFiles/os.dir/vx_mutex.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/os.dir/vx_mutex.c.i"
	cd /home/encheng/code_ui/ui/ui/BUILD/arbe_gui/os && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/encheng/code_ui/ui/ui/src/arbe_gui/os/vx_mutex.c > CMakeFiles/os.dir/vx_mutex.c.i

arbe_gui/os/CMakeFiles/os.dir/vx_mutex.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/os.dir/vx_mutex.c.s"
	cd /home/encheng/code_ui/ui/ui/BUILD/arbe_gui/os && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/encheng/code_ui/ui/ui/src/arbe_gui/os/vx_mutex.c -o CMakeFiles/os.dir/vx_mutex.c.s

arbe_gui/os/CMakeFiles/os.dir/vx_queue.c.o: arbe_gui/os/CMakeFiles/os.dir/flags.make
arbe_gui/os/CMakeFiles/os.dir/vx_queue.c.o: /home/encheng/code_ui/ui/ui/src/arbe_gui/os/vx_queue.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/encheng/code_ui/ui/ui/BUILD/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object arbe_gui/os/CMakeFiles/os.dir/vx_queue.c.o"
	cd /home/encheng/code_ui/ui/ui/BUILD/arbe_gui/os && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/os.dir/vx_queue.c.o   -c /home/encheng/code_ui/ui/ui/src/arbe_gui/os/vx_queue.c

arbe_gui/os/CMakeFiles/os.dir/vx_queue.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/os.dir/vx_queue.c.i"
	cd /home/encheng/code_ui/ui/ui/BUILD/arbe_gui/os && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/encheng/code_ui/ui/ui/src/arbe_gui/os/vx_queue.c > CMakeFiles/os.dir/vx_queue.c.i

arbe_gui/os/CMakeFiles/os.dir/vx_queue.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/os.dir/vx_queue.c.s"
	cd /home/encheng/code_ui/ui/ui/BUILD/arbe_gui/os && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/encheng/code_ui/ui/ui/src/arbe_gui/os/vx_queue.c -o CMakeFiles/os.dir/vx_queue.c.s

arbe_gui/os/CMakeFiles/os.dir/vx_task.c.o: arbe_gui/os/CMakeFiles/os.dir/flags.make
arbe_gui/os/CMakeFiles/os.dir/vx_task.c.o: /home/encheng/code_ui/ui/ui/src/arbe_gui/os/vx_task.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/encheng/code_ui/ui/ui/BUILD/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object arbe_gui/os/CMakeFiles/os.dir/vx_task.c.o"
	cd /home/encheng/code_ui/ui/ui/BUILD/arbe_gui/os && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/os.dir/vx_task.c.o   -c /home/encheng/code_ui/ui/ui/src/arbe_gui/os/vx_task.c

arbe_gui/os/CMakeFiles/os.dir/vx_task.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/os.dir/vx_task.c.i"
	cd /home/encheng/code_ui/ui/ui/BUILD/arbe_gui/os && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/encheng/code_ui/ui/ui/src/arbe_gui/os/vx_task.c > CMakeFiles/os.dir/vx_task.c.i

arbe_gui/os/CMakeFiles/os.dir/vx_task.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/os.dir/vx_task.c.s"
	cd /home/encheng/code_ui/ui/ui/BUILD/arbe_gui/os && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/encheng/code_ui/ui/ui/src/arbe_gui/os/vx_task.c -o CMakeFiles/os.dir/vx_task.c.s

# Object files for target os
os_OBJECTS = \
"CMakeFiles/os.dir/os_autogen/mocs_compilation.cpp.o" \
"CMakeFiles/os.dir/vx_event.c.o" \
"CMakeFiles/os.dir/vx_mutex.c.o" \
"CMakeFiles/os.dir/vx_queue.c.o" \
"CMakeFiles/os.dir/vx_task.c.o"

# External object files for target os
os_EXTERNAL_OBJECTS =

/home/encheng/code_ui/ui/ui/devel/lib/libos.so: arbe_gui/os/CMakeFiles/os.dir/os_autogen/mocs_compilation.cpp.o
/home/encheng/code_ui/ui/ui/devel/lib/libos.so: arbe_gui/os/CMakeFiles/os.dir/vx_event.c.o
/home/encheng/code_ui/ui/ui/devel/lib/libos.so: arbe_gui/os/CMakeFiles/os.dir/vx_mutex.c.o
/home/encheng/code_ui/ui/ui/devel/lib/libos.so: arbe_gui/os/CMakeFiles/os.dir/vx_queue.c.o
/home/encheng/code_ui/ui/ui/devel/lib/libos.so: arbe_gui/os/CMakeFiles/os.dir/vx_task.c.o
/home/encheng/code_ui/ui/ui/devel/lib/libos.so: arbe_gui/os/CMakeFiles/os.dir/build.make
/home/encheng/code_ui/ui/ui/devel/lib/libos.so: arbe_gui/os/CMakeFiles/os.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/encheng/code_ui/ui/ui/BUILD/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Linking CXX shared library /home/encheng/code_ui/ui/ui/devel/lib/libos.so"
	cd /home/encheng/code_ui/ui/ui/BUILD/arbe_gui/os && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/os.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
arbe_gui/os/CMakeFiles/os.dir/build: /home/encheng/code_ui/ui/ui/devel/lib/libos.so

.PHONY : arbe_gui/os/CMakeFiles/os.dir/build

arbe_gui/os/CMakeFiles/os.dir/clean:
	cd /home/encheng/code_ui/ui/ui/BUILD/arbe_gui/os && $(CMAKE_COMMAND) -P CMakeFiles/os.dir/cmake_clean.cmake
.PHONY : arbe_gui/os/CMakeFiles/os.dir/clean

arbe_gui/os/CMakeFiles/os.dir/depend:
	cd /home/encheng/code_ui/ui/ui/BUILD && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/encheng/code_ui/ui/ui/src /home/encheng/code_ui/ui/ui/src/arbe_gui/os /home/encheng/code_ui/ui/ui/BUILD /home/encheng/code_ui/ui/ui/BUILD/arbe_gui/os /home/encheng/code_ui/ui/ui/BUILD/arbe_gui/os/CMakeFiles/os.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : arbe_gui/os/CMakeFiles/os.dir/depend

