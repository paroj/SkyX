#-------------------------------------------------------------------
# This file is part of the CMake build system for SKYX
#
# The contents of this file are placed in the public domain. Feel
# free to make use of it in any way you like.
#-------------------------------------------------------------------

#######################################################################
# Find all necessary and optional SKYX dependencies
#######################################################################

# SKYX_DEPENDENCIES_DIR can be used to specify a single base
# folder where the required dependencies may be found.
set(SKYX_DEPENDENCIES_DIR "" CACHE PATH "Path to prebuilt SKYX dependencies")
include(SkyXFindPkgMacros)

set(SKYX_DEP_SEARCH_PATH
	${SKYX_DEPENDENCIES_DIR}
	${ENV_SKYX_DEPENDENCIES_DIR}
	${ENV_OGRE_HOME}
)
message(STATUS "Search path: ${SKYX_DEP_SEARCH_PATH}")

# Set hardcoded path guesses for various platforms
if (UNIX)
	set(SKYX_DEP_SEARCH_PATH ${SKYX_DEP_SEARCH_PATH}
	                         /usr/local)
endif ()

# give guesses as hints to the find_package calls
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${SKYX_DEP_SEARCH_PATH})
set(CMAKE_FRAMEWORK_PATH ${CMAKE_FRAMEWORK_PATH} ${SKYX_DEP_SEARCH_PATH})

message(STATUS "CMAKE_MODULE_PATH in SkyXDependencies = ${CMAKE_MODULE_PATH}")

#######################################################################
# Core dependencies
#######################################################################

# Find Ogre 3D, plus terrain and paging components
find_package(OGRE COMPONENTS)
macro_log_feature(OGRE_FOUND "OGRE" "3D library needed for the OgreGraphics plugin" "http://" TRUE "" "")

#######################################################################
# Tools dependencies
#######################################################################

# Find Doxygen
find_package(Doxygen)
macro_log_feature(DOXYGEN_FOUND "Doxygen" "Tool for building API documentation" "http://doxygen.org" FALSE "" "")

#######################################################################
# All dependencies were checked
#######################################################################

# Display results, terminate if anything required is missing
MACRO_DISPLAY_FEATURE_LOG()

# Add library and include paths from the dependencies
include_directories(
	${OGRE_INCLUDE_DIRS}
	${OGRE_INCLUDE_DIRS}/Overlay
	${Boost_INCLUDE_DIRS}
)

link_directories(
	${OGRE_LIBRARY_DIRS}
	${Boost_LIBRARY_DIRS}
)

