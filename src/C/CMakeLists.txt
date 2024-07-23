cmake_minimum_required (VERSION 3.14)

#############################
# Load CMake provided modules
include(CheckIncludeFile)
include(CheckIPOSupported)
include(CheckCCompilerFlag)
include(CheckPIESupported)
include(CMakePrintHelpers)
include(FeatureSummary)
include(GNUInstallDirs)

#############################
# Set C standards
enable_language(C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

#############################
# Test rationality
CHECK_C_SOURCE_COMPILES("int main(void) { return 0; } " CAN_COMPILE)
if (NOT CAN_COMPILE)
  message(FATAL_ERROR "Compiler non functional")
endif (NOT CAN_COMPILE)

message(STATUS "C Compiler ${CMAKE_C_COMPILER}")
message(STATUS " Supported C features = ${CMAKE_C_COMPILE_FEATURES}")

#############################
# Add our feature options
CHECK_INCLUDE_FILE(sys/capability.h HAVE_CAPABILITIES_H)
if (NOT HAVE_CAPABILITIES_H)
  message(FATAL_ERROR "sys/capability.h requested, but not found")
endif (NOT HAVE_CAPABILITIES_H)

#############################
# Set Code position
check_pie_supported(OUTPUT_VARIABLE output LANGUAGES C)
if(CMAKE_C_LINK_PIE_SUPPORTED)
  set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
else()
  message(WARNING "PIE is not supported at link time: ${output}.\n"
                  "PIE link options will not be passed to linker.")
endif(CMAKE_C_LINK_PIE_SUPPORTED)

check_ipo_supported(RESULT IPO_RESULT)
if(IPO_RESULT)
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
  message(WARNING "Interprocedural Optimization is not supported at link time: ${output}.\n"
                  "Interprocedural Optimization will not be passed to linker.")
endif(IPO_RESULT)

#############################
# Add required warnings level
add_compile_options(-Wall -Wpedantic -Wconversion -Wformat -Wformat-security -Wstrict-prototypes -Wstrict-overflow -Werror=implicit-function-declaration -Wextra -Werror)

#############################
# Permit sound debugging tables
CHECK_C_COMPILER_FLAG(-fasynchronous-unwind-tables UNWIND_TABLES)
if (UNWIND_TABLES)
  add_compile_options(-fasynchronous-unwind-tables)
else ()
  message(WARNING "Expected compile flag '-fasynchronous-unwind-tables' not found")
endif (UNWIND_TABLES)

#############################
# Most of our internal functions can be inlined
CHECK_C_COMPILER_FLAG(-finline-functions INLINE_FUNCTIONS)
if (INLINE_FUNCTIONS)
  add_compile_options(-finline-functions)
else ()
  message(WARNING "Expected compile flag '-finline-functions' not found")
endif (INLINE_FUNCTIONS)

#############################
# Annotate binaries
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
  CHECK_C_COMPILER_FLAG(-fplugin=annobin SET_ANNOBIN)
  if (SET_ANNOBIN)
    add_compile_options(-fplugin=annobin)
  else ()
    message(WARNING "Expected compile flag '-fplugin=annobin' not found")
  endif (SET_ANNOBIN)
  CHECK_C_COMPILER_FLAG(-grecord-gcc-switches SET_GCC_SWITCHES)
  if (SET_GCC_SWITCHES)
    add_compile_options(-grecord-gcc-switches)
  else ()
    message(WARNING "Expected compile flag '-grecord-gcc-switches' not found")
  endif (SET_GCC_SWITCHES)
endif (CMAKE_C_COMPILER_ID STREQUAL "GNU")

#############################
# Ensure the linker is hardened
add_link_options(-Wl,-z,defs -Wl,-z,noexecstack -Wl,-z,nodump -Wl,-z,relro -Wl,-z,now -Wl,-pie -Wl,-z,combreloc)

#############################
# Use optimization by default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RELWITHDEBINFO)
endif(NOT CMAKE_BUILD_TYPE)

#############################
# Harden the compiler

if (CMAKE_BUILD_TYPE EQUAL "DEBUG")
    set(DEBUG TRUE)
else()
    # Add -D_FORTIFY_SOURCE=2 hardening flags, these only work when -O is passed
    add_compile_options(-D_FORTIFY_SOURCE=2)
endif (CMAKE_BUILD_TYPE EQUAL "DEBUG")

# ssp-buffer-size has a minor performance hit, but eh
add_compile_options(--param=ssp-buffer-size=4)

CHECK_C_COMPILER_FLAG(-fexceptions C_EXCEPTIONS)
if (C_EXCEPTIONS)
  add_compile_options(-fexceptions)
else ()
  message(FATAL_ERROR "Required compile flag '-fexceptions' not found")
endif (C_EXCEPTIONS)

CHECK_C_COMPILER_FLAG(-fstack-clash-protection STACK_CLASH)
if (STACK_CLASH)
  add_compile_options(-fstack-clash-protection)
else ()
  message(WARNING "Expected compile flag '-fstack-clash-protection' not found")
endif (STACK_CLASH)

CHECK_C_COMPILER_FLAG(-fstack-reuse=none STACK_REUSE)
if (STACK_REUSE)
  add_compile_options(-fstack-reuse=none)
else ()
  message(FATAL_ERROR "Required compile flag '-fstack-reuse=none' not found")
endif (STACK_REUSE)

CHECK_C_COMPILER_FLAG(-fstack-protector-all STACK_PROTECT)
if (STACK_PROTECT)
  add_compile_options(-fstack-protector-all)
else ()
  message(FATAL_ERROR "Required compile flag '-fstack-protector-all' not found")
endif (STACK_PROTECT)

CHECK_C_COMPILER_FLAG(-fcf-protection=full FCF_PROTECT)
if (FCF_PROTECT)
  add_compile_options(-fcf-protection=full)
else ()
  message(WARNING "Expected compile flag '-fcf-protection=full' not found")
endif (FCF_PROTECT)

CHECK_C_COMPILER_FLAG(-fzero-call-used-regs=all ZERO_CALL_REGS)
if (ZERO_CALL_REGS)
  add_compile_options(-fzero-call-used-regs=all)
else ()
  message(WARNING "Expected compile flag '-fzero-call-used-regs=all' not found")
endif (ZERO_CALL_REGS)

CHECK_C_COMPILER_FLAG(-mshstk SAFESTACK_SHADOW)
if (SAFESTACK_SHADOW)
  add_compile_options(-mshstk)
else ()
  message(WARNING "Expected compile flag '-mshstk' not found")
endif (SAFESTACK_SHADOW)

#############################
# Our build targets
add_executable(ambient_cap_net_raw)

#############################
# Setup install target
install(TARGETS ambient_cap_net_raw DESTINATION ${CMAKE_INSTALL_LIBEXECDIR})

#############################
# Our build targets specific options
target_compile_features(ambient_cap_net_raw PRIVATE c_std_11)
target_compile_features(ambient_cap_net_raw PRIVATE c_restrict)
target_compile_features(ambient_cap_net_raw PRIVATE c_function_prototypes)
target_compile_features(ambient_cap_net_raw PRIVATE c_static_assert)

target_link_libraries(ambient_cap_net_raw PRIVATE cap)

target_sources(ambient_cap_net_raw PRIVATE ${PROJECT_SOURCE_DIR}/src/C/ambient_cap_net_raw.c)
