cmake_minimum_required (VERSION 3.14)

project(ambient_cap_net_raw C)

set(LANGUAGES "C")

execute_process(COMMAND git describe --abbrev=0 OUTPUT_VARIABLE VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)

####
# CMake modules
include(FeatureSummary)

set_package_properties(kcron PROPERTIES
 URL "https://github.com/fermitools/ambient_cap_net_raw"
 DESCRIPTION "A utility for running a program wih cap_net_raw enabled")

set(CPACK_PACKAGE_NAME "fermilab-util_ambient_cap_net_raw")
set(CPACK_PACKAGE_CONTACT "fermilab <servicedesk@fnal.gov>")
set(CPACK_PACKAGE_DESCRIPTION A utility for running a program wih cap_net_raw enabled)
set(CPACK_PACKAGE_VERSION ${VERSION})
set(CPACK_PACKAGE_RELEASE 1)

set(CPACK_RESOURCE_FILE_LICENSE ${PROJECT_SOURCE_DIR}/LICENSE)
set(CPACK_RPM_PACKAGE_LICENSE GPLv3+)
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")
set(CPACK_RPM_PACKAGE_RELOCATABLE OFF)
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")

set(CPACK_GENERATOR "RPM")
include(CPack)

### Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory and run cmake from there.")
endif(EXISTS "${LOC_PATH}")

####
# My CMake files
include(src/C/CMakeLists.txt)

####
# Print out feature summary
message("${CMAKE_PROJECT_NAME} - ${VERSION}")
feature_summary(WHAT ALL)
feature_summary(FILENAME ${CMAKE_CURRENT_BINARY_DIR}/features.log WHAT ALL)
