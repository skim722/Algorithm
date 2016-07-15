CMAKE_MINIMUM_REQUIRED( VERSION 2.8.0 )
PROJECT( CSE6140 CXX )
SET( _CMAKE_PROJECT_INITIALS "CSE6140" CACHE INTERNAL "Project Name Initials" )

if (NOT WIN32)
  string(ASCII 27 Esc)
  SET(ColourReset       "${Esc}[m")
  SET(ColourBold        "${Esc}[1m")
  SET(Red               "${Esc}[31m")
  SET(Green             "${Esc}[32m")
  SET(Yellow            "${Esc}[33m")
  SET(Blue              "${Esc}[34m")
  SET(Magenta           "${Esc}[35m")
  SET(Cyan              "${Esc}[36m")
  SET(White             "${Esc}[37m")
  SET(BoldRed           "${Esc}[1;31m")
  SET(BoldGreen         "${Esc}[1;32m")
  SET(BoldYellow        "${Esc}[1;33m")
  SET(BoldBlue          "${Esc}[1;34m")
  SET(BoldMagenta       "${Esc}[1;35m")
  SET(BoldCyan          "${Esc}[1;36m")
  SET(BoldWhite         "${Esc}[1;37m")
  SET(UnderlinedRed     "${Esc}[4;31m")
endif()

MESSAGE( "\n${BoldCyan}    **** WELCOME TO ${CMAKE_PROJECT_NAME} (${_CMAKE_PROJECT_INITIALS})! ****${ColourReset}\n" )

# Use C++11
INCLUDE(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++14" COMPILER_SUPPORTS_CXX14)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if (COMPILER_SUPPORTS_CXX14)
    MESSAGE(STATUS "The installed ${CMAKE_CXX_COMPILER_ID} has C++14 support!")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
elseif (COMPILER_SUPPORTS_CXX11)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif (COMPILER_SUPPORTS_CXX0X)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else ()
    MESSAGE(FATAL_ERROR "${BoldRed}This project requires C++11 and the installed ${CMAKE_CXX_COMPILER_ID} has no C++11 support. Please install a modern version of ${CMAKE_CXX_COMPILER_ID}!${ColourReset}")
endif()

# On Linux machines, clang uses gcc's libstdc++ library by default
# External libraries built using libstdc++ will need to be rebuilt with libc++ though - see http://stackoverflow.com/questions/8454329/why-cant-clang-with-libc-in-c0x-mode-link-this-boostprogram-options-examp
CHECK_CXX_COMPILER_FLAG("-stdlib=libc++" COMPILER_SUPPORTS_LIBCXX)
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND COMPILER_SUPPORTS_LIBCXX)
    MESSAGE( STATUS "libc++ found; explicitly compiling with -stdlib=libc++..." )
    MESSAGE( WARNING "${UnderlinedRed}External dependencies may need to be rebuilt using libc++ to avoid linker errors during project compilation!${ColourReset}" )
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

# http://stackoverflow.com/questions/10046114/in-cmake-how-can-i-test-if-the-compiler-is-clang
# http://www.cmake.org/cmake/help/v3.0/policy/CMP0025.html#policy:CMP0025

# Older versions of compilers may support C++11, but not include C++11 standards-compliant libraries
if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.0)
    # GCC's libstdc++ implementation is missing many features prior to GCC 5
    # For example, the <regex> library is not included in GCC versions prior to 4.9
    # Move semantics are missing in parts of the library prior to GCC 5 - i.e. see http://stackoverflow.com/questions/28775673/why-cant-i-move-stdofstream
    MESSAGE( FATAL_ERROR "${BoldRed}GCC version must be >= 5.0; only GCC 5.0+ comes with a fully C++11-compliant standard library!${ColourReset}" )
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.1)
    # http://stackoverflow.com/questions/12015899/why-are-move-semantics-for-a-class-containing-a-stdstringstream-causing-compil
    MESSAGE( FATAL_ERROR "${BoldRed}Clang version must be >= 3.1; only Clang 3.1+ comes with a fully C++11-compliant standard library!${ColourReset}" )
endif()

# Enable compiler warnings
if (MSVC)
    if (CMAKE_CXX_FLAGS MATCHES "/W[0-4]") # Force to always compile with W4
        STRING(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else ()
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif()
elseif (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")
endif()

INCLUDE_DIRECTORIES(src)

# Build project in the following subdirectories
ADD_SUBDIRECTORY(src)


ADD_EXECUTABLE(sample sample.cpp)
TARGET_LINK_LIBRARIES(sample CSE6140Utilities)

ADD_EXECUTABLE(runGA runGA.cpp)
TARGET_LINK_LIBRARIES(runGA CSE6140Utilities)

# Remove all files not tracked by git
ADD_CUSTOM_TARGET(gitclean COMMAND git clean -d -f -x)
MESSAGE( "\n${Magenta}    To clean out all files not tracked by git,\n    simply run 'make gitclean' from the\n    project ROOT directory.${ColourReset}\n" )
