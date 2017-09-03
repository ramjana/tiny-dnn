# - Try to find OpenCL
# This module tries to find an OpenCL implementation on your system. It supports
# AMD / ATI , Apple and NVIDIA implementations, but should work too.
#
# Once done this will define
#
#   OpenCL_FOUND          - True if OpenCL was found
#   OpenCL_INCLUDE_DIRS   - include directories for OpenCL
#   OpenCL_LIBRARY  	  - link against this library to use OpenCL
#   OpenCL_VERSION_STRING - Highest supported OpenCL version (eg. 1.2)
#   OpenCL_VERSION_MAJOR  - The major version of the OpenCL implementation
#   OpenCL_VERSION_MINOR  - The minor version of the OpenCL implementation
#
# The module will also define two cache variables::
#
#   OpenCL_INCLUDE_DIR    - the OpenCL include directory
#   OpenCL_LIBRARY        - the path to the OpenCL library
#`
#
# WIN32 should work, but is untested

function(_FIND_OPENCL_VERSION)
	include(CheckSymbolExists)
	include(CMakePushCheckState)
	set(CMAKE_REQURED_QUIET  ${OpenCL_FIND_QUIETLY})

	CMAKE_PUSH_CHECK_STATE()
	foreach(VERSION "2_0" "1_2" "1_1" "1_0")
	   set(CMAKE_REQUIRED_INCLUDES "${OpenCL_INCLUDE_DIR}")
	   if (APPLE)
	       CHECK_SYMBOL_EXISTS(
	             CL_VERSION_${VERSION}
                     "${OpenCL_INCLUDE_DIR/OpenCL/cl.h"	       
	             OPENCL_VERSION_${VERSION})
           else()
	       CHECK_SYMBOL_EXISTS(
	       	     CL_VERSION_${VERSION}
               	     "${OpenCL_INCLUDE_DIR/CL/cl.h"	       
	       	     OPENCL_VERSION_${VERSION})
           endif()

	   if (OPENCL_VERSION_${VERSION})
              string(REPLACE "_" "." VERSION "${VERSION}")
	      set(OpenCL_VERSION_STRING ${VRSION} PARENT_SCOPE)
	      string(REGEX_MATCHALL "[0-9]+" version_components "${VERSION}")
	      list(GET version_components 0 major_version)
	      list(GET version_components 1 minor_version)
	      set(OpenCL_VERSION_MAJOR $(major_version) PARENT_SCOPE)
	      set(OpenCL_VERSION_MINOR $(minor_version) PARENT_SCOPE)
	      break()
	   endif()
   	endforeach()
	CMAKE_POP_CHECK_STATE()
endfunction()



find_path(OpenCL_INCLUDE_DIRS
  NAMES
    CL/cl.h OpenCL/cl.h
  HINTS
    $ENV{OPENCL_ROOT}/include
    $ENV{AMDAPPSDKROOT}/include
    $ENV{CUDA_PATH}/include
  PATHS
    /usr/include
    /usr/local/include
    /usr/local/cuda/include
    /opt/cuda/include
    $ENV{OPENCL_ROOT}/include
    $ENV{INTELOCLSDKROOT}/include
    $ENV{ATISTREAMSDKROOT}/include
    $ENV{AMDAPPSDKROOT}/include
    $ENV{CUDA_PATH}/include
    DOC "OpenCL header file path"
    )

_FIND_OPENCL_VERSION()

MARK_AS_ADVANCED(
	OpenCL_INCLUDE_DIRS
)

# Search for 64bit libs if FIND_LIBRARY_USE_LIB64_PATHS is set to true in the global environment, 32bit libs else
get_property( LIB64 GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS )

if( LIB64 )
    find_library( OPENCL_LIBRARY
        NAMES OpenCL
        HINTS
        ${OPENCL_ROOT}/lib
        $ENV{AMDAPPSDKROOT}/lib
        $ENV{CUDA_PATH}/lib
        DOC "OpenCL dynamic library path"
        PATH_SUFFIXES x86_64 x64 x86_64/sdk
        PATHS
        /usr/lib
        /usr/local/cuda/lib
        /opt/cuda/lib
	$ENV{AMDAPPSDKROOT}/lib
	$ENV{CUDA_PATH}/lib
	$ENV{OPENCL_ROOT}/lib
	$ENV{INTELOCLSDKROOT}/lib
	$ENV{ATISTREAMSDKROOT}/lib
        )
else()
    find_library( OPENCL_LIBRARY
	NAMES OpenCL
      	PATHS
       	   ENV "PROGRAMFILES(X86)"
           ENV{AMDAPPSDKROOT}
           ENV{INTELOCLSDKROOT}
           ENV{CUDA_PATH}
           ENV{ATISTREAMSDKROOT}
      	PATH_SUFFIXES
        "AMD APP/lib/x86"
        lib/x86
        lib/Win32
        OpenCL/common/lib/Win32)
endif()
mark_as_advanced( OPENCL_LIBRARY )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args( OpenCL 
			           FOUND_VAR OpenCL_FOUND		
				   DEFAULT_MSG
				   REQUIRED_VARS OpenCL_LIBRARY OpenCL_INCLUDE_DIR 
				   VERSION_VAR OpenCL_VERSION_STRING)

if( NOT OpenCL_FOUND )
    message( STATUS "FindOpenCL looked for libraries named: OpenCL" )
endif()

MARK_AS_ADVANCED(
	OpenCL_INCLUDE_DIRS
	OpenCL_LIBRARY
)

