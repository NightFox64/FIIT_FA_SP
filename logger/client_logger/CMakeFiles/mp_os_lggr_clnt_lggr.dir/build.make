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
include logger/client_logger/CMakeFiles/mp_os_lggr_clnt_lggr.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include logger/client_logger/CMakeFiles/mp_os_lggr_clnt_lggr.dir/compiler_depend.make

# Include the progress variables for this target.
include logger/client_logger/CMakeFiles/mp_os_lggr_clnt_lggr.dir/progress.make

# Include the compile flags for this target's objects.
include logger/client_logger/CMakeFiles/mp_os_lggr_clnt_lggr.dir/flags.make

logger/client_logger/CMakeFiles/mp_os_lggr_clnt_lggr.dir/codegen:
.PHONY : logger/client_logger/CMakeFiles/mp_os_lggr_clnt_lggr.dir/codegen

logger/client_logger/CMakeFiles/mp_os_lggr_clnt_lggr.dir/src/client_logger.cpp.o: logger/client_logger/CMakeFiles/mp_os_lggr_clnt_lggr.dir/flags.make
logger/client_logger/CMakeFiles/mp_os_lggr_clnt_lggr.dir/src/client_logger.cpp.o: logger/client_logger/src/client_logger.cpp
logger/client_logger/CMakeFiles/mp_os_lggr_clnt_lggr.dir/src/client_logger.cpp.o: logger/client_logger/CMakeFiles/mp_os_lggr_clnt_lggr.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/root/FIIT_FA_SP/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object logger/client_logger/CMakeFiles/mp_os_lggr_clnt_lggr.dir/src/client_logger.cpp.o"
	cd /root/FIIT_FA_SP/logger/client_logger && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT logger/client_logger/CMakeFiles/mp_os_lggr_clnt_lggr.dir/src/client_logger.cpp.o -MF CMakeFiles/mp_os_lggr_clnt_lggr.dir/src/client_logger.cpp.o.d -o CMakeFiles/mp_os_lggr_clnt_lggr.dir/src/client_logger.cpp.o -c /root/FIIT_FA_SP/logger/client_logger/src/client_logger.cpp

logger/client_logger/CMakeFiles/mp_os_lggr_clnt_lggr.dir/src/client_logger.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/mp_os_lggr_clnt_lggr.dir/src/client_logger.cpp.i"
	cd /root/FIIT_FA_SP/logger/client_logger && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/FIIT_FA_SP/logger/client_logger/src/client_logger.cpp > CMakeFiles/mp_os_lggr_clnt_lggr.dir/src/client_logger.cpp.i

logger/client_logger/CMakeFiles/mp_os_lggr_clnt_lggr.dir/src/client_logger.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/mp_os_lggr_clnt_lggr.dir/src/client_logger.cpp.s"
	cd /root/FIIT_FA_SP/logger/client_logger && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/FIIT_FA_SP/logger/client_logger/src/client_logger.cpp -o CMakeFiles/mp_os_lggr_clnt_lggr.dir/src/client_logger.cpp.s

logger/client_logger/CMakeFiles/mp_os_lggr_clnt_lggr.dir/src/client_logger_builder.cpp.o: logger/client_logger/CMakeFiles/mp_os_lggr_clnt_lggr.dir/flags.make
logger/client_logger/CMakeFiles/mp_os_lggr_clnt_lggr.dir/src/client_logger_builder.cpp.o: logger/client_logger/src/client_logger_builder.cpp
logger/client_logger/CMakeFiles/mp_os_lggr_clnt_lggr.dir/src/client_logger_builder.cpp.o: logger/client_logger/CMakeFiles/mp_os_lggr_clnt_lggr.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/root/FIIT_FA_SP/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object logger/client_logger/CMakeFiles/mp_os_lggr_clnt_lggr.dir/src/client_logger_builder.cpp.o"
	cd /root/FIIT_FA_SP/logger/client_logger && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT logger/client_logger/CMakeFiles/mp_os_lggr_clnt_lggr.dir/src/client_logger_builder.cpp.o -MF CMakeFiles/mp_os_lggr_clnt_lggr.dir/src/client_logger_builder.cpp.o.d -o CMakeFiles/mp_os_lggr_clnt_lggr.dir/src/client_logger_builder.cpp.o -c /root/FIIT_FA_SP/logger/client_logger/src/client_logger_builder.cpp

logger/client_logger/CMakeFiles/mp_os_lggr_clnt_lggr.dir/src/client_logger_builder.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/mp_os_lggr_clnt_lggr.dir/src/client_logger_builder.cpp.i"
	cd /root/FIIT_FA_SP/logger/client_logger && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/FIIT_FA_SP/logger/client_logger/src/client_logger_builder.cpp > CMakeFiles/mp_os_lggr_clnt_lggr.dir/src/client_logger_builder.cpp.i

logger/client_logger/CMakeFiles/mp_os_lggr_clnt_lggr.dir/src/client_logger_builder.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/mp_os_lggr_clnt_lggr.dir/src/client_logger_builder.cpp.s"
	cd /root/FIIT_FA_SP/logger/client_logger && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/FIIT_FA_SP/logger/client_logger/src/client_logger_builder.cpp -o CMakeFiles/mp_os_lggr_clnt_lggr.dir/src/client_logger_builder.cpp.s

# Object files for target mp_os_lggr_clnt_lggr
mp_os_lggr_clnt_lggr_OBJECTS = \
"CMakeFiles/mp_os_lggr_clnt_lggr.dir/src/client_logger.cpp.o" \
"CMakeFiles/mp_os_lggr_clnt_lggr.dir/src/client_logger_builder.cpp.o"

# External object files for target mp_os_lggr_clnt_lggr
mp_os_lggr_clnt_lggr_EXTERNAL_OBJECTS =

logger/client_logger/libmp_os_lggr_clnt_lggr.a: logger/client_logger/CMakeFiles/mp_os_lggr_clnt_lggr.dir/src/client_logger.cpp.o
logger/client_logger/libmp_os_lggr_clnt_lggr.a: logger/client_logger/CMakeFiles/mp_os_lggr_clnt_lggr.dir/src/client_logger_builder.cpp.o
logger/client_logger/libmp_os_lggr_clnt_lggr.a: logger/client_logger/CMakeFiles/mp_os_lggr_clnt_lggr.dir/build.make
logger/client_logger/libmp_os_lggr_clnt_lggr.a: logger/client_logger/CMakeFiles/mp_os_lggr_clnt_lggr.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/root/FIIT_FA_SP/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX static library libmp_os_lggr_clnt_lggr.a"
	cd /root/FIIT_FA_SP/logger/client_logger && $(CMAKE_COMMAND) -P CMakeFiles/mp_os_lggr_clnt_lggr.dir/cmake_clean_target.cmake
	cd /root/FIIT_FA_SP/logger/client_logger && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/mp_os_lggr_clnt_lggr.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
logger/client_logger/CMakeFiles/mp_os_lggr_clnt_lggr.dir/build: logger/client_logger/libmp_os_lggr_clnt_lggr.a
.PHONY : logger/client_logger/CMakeFiles/mp_os_lggr_clnt_lggr.dir/build

logger/client_logger/CMakeFiles/mp_os_lggr_clnt_lggr.dir/clean:
	cd /root/FIIT_FA_SP/logger/client_logger && $(CMAKE_COMMAND) -P CMakeFiles/mp_os_lggr_clnt_lggr.dir/cmake_clean.cmake
.PHONY : logger/client_logger/CMakeFiles/mp_os_lggr_clnt_lggr.dir/clean

logger/client_logger/CMakeFiles/mp_os_lggr_clnt_lggr.dir/depend:
	cd /root/FIIT_FA_SP && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/FIIT_FA_SP /root/FIIT_FA_SP/logger/client_logger /root/FIIT_FA_SP /root/FIIT_FA_SP/logger/client_logger /root/FIIT_FA_SP/logger/client_logger/CMakeFiles/mp_os_lggr_clnt_lggr.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : logger/client_logger/CMakeFiles/mp_os_lggr_clnt_lggr.dir/depend

