# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 4.0

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /root/FIIT_FA_SP

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /root/FIIT_FA_SP

# Include any dependencies generated for this target.
include arithmetic/continued_fraction/CMakeFiles/mp_os_arthmtc_cntnd_frctn.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include arithmetic/continued_fraction/CMakeFiles/mp_os_arthmtc_cntnd_frctn.dir/compiler_depend.make

# Include the progress variables for this target.
include arithmetic/continued_fraction/CMakeFiles/mp_os_arthmtc_cntnd_frctn.dir/progress.make

# Include the compile flags for this target's objects.
include arithmetic/continued_fraction/CMakeFiles/mp_os_arthmtc_cntnd_frctn.dir/flags.make

arithmetic/continued_fraction/CMakeFiles/mp_os_arthmtc_cntnd_frctn.dir/codegen:
.PHONY : arithmetic/continued_fraction/CMakeFiles/mp_os_arthmtc_cntnd_frctn.dir/codegen

arithmetic/continued_fraction/CMakeFiles/mp_os_arthmtc_cntnd_frctn.dir/src/continued_fraction.cpp.o: arithmetic/continued_fraction/CMakeFiles/mp_os_arthmtc_cntnd_frctn.dir/flags.make
arithmetic/continued_fraction/CMakeFiles/mp_os_arthmtc_cntnd_frctn.dir/src/continued_fraction.cpp.o: arithmetic/continued_fraction/src/continued_fraction.cpp
arithmetic/continued_fraction/CMakeFiles/mp_os_arthmtc_cntnd_frctn.dir/src/continued_fraction.cpp.o: arithmetic/continued_fraction/CMakeFiles/mp_os_arthmtc_cntnd_frctn.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/root/FIIT_FA_SP/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object arithmetic/continued_fraction/CMakeFiles/mp_os_arthmtc_cntnd_frctn.dir/src/continued_fraction.cpp.o"
	cd /root/FIIT_FA_SP/arithmetic/continued_fraction && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT arithmetic/continued_fraction/CMakeFiles/mp_os_arthmtc_cntnd_frctn.dir/src/continued_fraction.cpp.o -MF CMakeFiles/mp_os_arthmtc_cntnd_frctn.dir/src/continued_fraction.cpp.o.d -o CMakeFiles/mp_os_arthmtc_cntnd_frctn.dir/src/continued_fraction.cpp.o -c /root/FIIT_FA_SP/arithmetic/continued_fraction/src/continued_fraction.cpp

arithmetic/continued_fraction/CMakeFiles/mp_os_arthmtc_cntnd_frctn.dir/src/continued_fraction.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/mp_os_arthmtc_cntnd_frctn.dir/src/continued_fraction.cpp.i"
	cd /root/FIIT_FA_SP/arithmetic/continued_fraction && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/FIIT_FA_SP/arithmetic/continued_fraction/src/continued_fraction.cpp > CMakeFiles/mp_os_arthmtc_cntnd_frctn.dir/src/continued_fraction.cpp.i

arithmetic/continued_fraction/CMakeFiles/mp_os_arthmtc_cntnd_frctn.dir/src/continued_fraction.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/mp_os_arthmtc_cntnd_frctn.dir/src/continued_fraction.cpp.s"
	cd /root/FIIT_FA_SP/arithmetic/continued_fraction && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/FIIT_FA_SP/arithmetic/continued_fraction/src/continued_fraction.cpp -o CMakeFiles/mp_os_arthmtc_cntnd_frctn.dir/src/continued_fraction.cpp.s

# Object files for target mp_os_arthmtc_cntnd_frctn
mp_os_arthmtc_cntnd_frctn_OBJECTS = \
"CMakeFiles/mp_os_arthmtc_cntnd_frctn.dir/src/continued_fraction.cpp.o"

# External object files for target mp_os_arthmtc_cntnd_frctn
mp_os_arthmtc_cntnd_frctn_EXTERNAL_OBJECTS =

arithmetic/continued_fraction/libmp_os_arthmtc_cntnd_frctn.a: arithmetic/continued_fraction/CMakeFiles/mp_os_arthmtc_cntnd_frctn.dir/src/continued_fraction.cpp.o
arithmetic/continued_fraction/libmp_os_arthmtc_cntnd_frctn.a: arithmetic/continued_fraction/CMakeFiles/mp_os_arthmtc_cntnd_frctn.dir/build.make
arithmetic/continued_fraction/libmp_os_arthmtc_cntnd_frctn.a: arithmetic/continued_fraction/CMakeFiles/mp_os_arthmtc_cntnd_frctn.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/root/FIIT_FA_SP/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libmp_os_arthmtc_cntnd_frctn.a"
	cd /root/FIIT_FA_SP/arithmetic/continued_fraction && $(CMAKE_COMMAND) -P CMakeFiles/mp_os_arthmtc_cntnd_frctn.dir/cmake_clean_target.cmake
	cd /root/FIIT_FA_SP/arithmetic/continued_fraction && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/mp_os_arthmtc_cntnd_frctn.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
arithmetic/continued_fraction/CMakeFiles/mp_os_arthmtc_cntnd_frctn.dir/build: arithmetic/continued_fraction/libmp_os_arthmtc_cntnd_frctn.a
.PHONY : arithmetic/continued_fraction/CMakeFiles/mp_os_arthmtc_cntnd_frctn.dir/build

arithmetic/continued_fraction/CMakeFiles/mp_os_arthmtc_cntnd_frctn.dir/clean:
	cd /root/FIIT_FA_SP/arithmetic/continued_fraction && $(CMAKE_COMMAND) -P CMakeFiles/mp_os_arthmtc_cntnd_frctn.dir/cmake_clean.cmake
.PHONY : arithmetic/continued_fraction/CMakeFiles/mp_os_arthmtc_cntnd_frctn.dir/clean

arithmetic/continued_fraction/CMakeFiles/mp_os_arthmtc_cntnd_frctn.dir/depend:
	cd /root/FIIT_FA_SP && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/FIIT_FA_SP /root/FIIT_FA_SP/arithmetic/continued_fraction /root/FIIT_FA_SP /root/FIIT_FA_SP/arithmetic/continued_fraction /root/FIIT_FA_SP/arithmetic/continued_fraction/CMakeFiles/mp_os_arthmtc_cntnd_frctn.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : arithmetic/continued_fraction/CMakeFiles/mp_os_arthmtc_cntnd_frctn.dir/depend

