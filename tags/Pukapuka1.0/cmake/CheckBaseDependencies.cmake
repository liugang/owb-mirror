IF(NOT WIN32)
    ## Pkg-config under cygwin gives to cmake .a lib and we need .lib for Visual Studio projects so we search them with cmake tools. 
    find_package(PkgConfig REQUIRED)
ENDIF(NOT WIN32)
find_package(Perl REQUIRED)
find_program(BISON_EXECUTABLE bison)
if(NOT BISON_EXECUTABLE)
    message(FATAL_ERROR "bison package not found. Install it to be able to compile owb.")
endif(NOT BISON_EXECUTABLE)
find_program(FLEX_EXECUTABLE flex)
if(NOT FLEX_EXECUTABLE)
    message(FATAL_ERROR "flex package not found. Install it to be able to compile owb.")
endif(NOT FLEX_EXECUTABLE)
find_program(GPERF_EXECUTABLE gperf)
if(NOT GPERF_EXECUTABLE)
    message(FATAL_ERROR "gperf package not found. Install it to be able to compile owb.")
endif(NOT GPERF_EXECUTABLE)

IF(NOT WIN32)
    pkg_check_modules(OWB_BASE_DEPS REQUIRED libxml-2.0>=2.6)
ELSE(NOT WIN32)
    ## Pkg-config under cygwin gives to cmake .a lib and we need .lib for Visual Studio projects. 
    find_path(LIB_XML2_PATH libxml2.lib ${WINLIB_LIB_PATH})
    find_path(INC_XML2 xmlwin32version.h ${WINLIB_INC_PATH} ${WINLIB_INC_PATH}/libxml)
    IF(NOT LIB_XML2_PATH)
        MESSAGE(FATAL_ERROR "XML lib not found.  Install it to be able to compile owb.")
    ELSE(NOT LIB_XML2_PATH)
        find_file(LIB_XML2 ${LIB_XML2_PATH}/libxml2.lib)
    ENDIF(NOT LIB_XML2_PATH)
ENDIF(NOT WIN32)