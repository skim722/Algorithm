# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.4

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
CMAKE_COMMAND = /Volumes/cmake-3.4.1-Darwin-x86_64/CMake.app/Contents/bin/cmake

# The command to remove a file.
RM = /Volumes/cmake-3.4.1-Darwin-x86_64/CMake.app/Contents/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/sookim/cse6140

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/sookim/cse6140

# Utility rule file for gitclean.

# Include the progress variables for this target.
include CMakeFiles/gitclean.dir/progress.make

CMakeFiles/gitclean:
	git clean -d -f -x

gitclean: CMakeFiles/gitclean
gitclean: CMakeFiles/gitclean.dir/build.make

.PHONY : gitclean

# Rule to build all files generated by this target.
CMakeFiles/gitclean.dir/build: gitclean

.PHONY : CMakeFiles/gitclean.dir/build

CMakeFiles/gitclean.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/gitclean.dir/cmake_clean.cmake
.PHONY : CMakeFiles/gitclean.dir/clean

CMakeFiles/gitclean.dir/depend:
	cd /Users/sookim/cse6140 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/sookim/cse6140 /Users/sookim/cse6140 /Users/sookim/cse6140 /Users/sookim/cse6140 /Users/sookim/cse6140/CMakeFiles/gitclean.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/gitclean.dir/depend
