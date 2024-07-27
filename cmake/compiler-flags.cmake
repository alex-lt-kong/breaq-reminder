# ##################################################################################################
# # COMPILER FLAGS #################################################################################
# ##################################################################################################

message(CMAKE_CXX_COMPILER_ID="${CMAKE_CXX_COMPILER_ID}")

# Hardening flags
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options("-O3")
    # add_compile_options("-g")
    add_compile_options("-Wall")
    add_compile_options("-pedantic")
    # See https://developers.redhat.com/blog/2018/03/21/compiler-and-linker-flags-gcc
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      add_compile_options("-D_GLIBCXX_ASSERTIONS")
      add_compile_options("-Wextra")
      add_compile_options("-fasynchronous-unwind-tables")
      add_compile_options("-fexceptions")
      add_compile_options("-fstack-clash-protection")
      add_compile_options("-fstack-protector-strong")
      add_compile_options("-grecord-gcc-switches")

      # Issue 872: https://github.com/oatpp/oatpp/issues/872
      # -fcf-protection is supported only on x86 GNU/Linux per this gcc doc:
      # https://gcc.gnu.org/onlinedocs/gcc/Instrumentation-Options.html#index-fcf-protection
      # add_compile_options("-fcf-protection")
      add_compile_options("-pipe")
      add_compile_options("-Werror=format-security")
      add_compile_options("-Wno-format-nonliteral")
      add_compile_options("-fPIE")
      add_compile_options("-Wl,-z,defs")
      add_compile_options("-Wl,-z,now")
      add_compile_options("-Wl,-z,relro")
    endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
  # using Intel C++
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options("/Ox")
    else()
        add_compile_options("/sdl")          # Enable Additional Security Checks
        add_compile_options("/guard:cf")     # Enable Control Flow Guard
        add_compile_options("/guard:ehcont") # Enable EH Continuation Metadata:  used during runtime for NtContinue and SetThreadContext instruction pointer validation.
        # add_compile_options("/CETCOMPAT")    # CET Shadow Stack compatible: provides capabilities to defend against return-oriented programming (ROP) based malware attacks
    endif()
    # https://stackoverflow.com/questions/399850/best-compiler-warning-level-for-c-c-compilers/399865#
    # https://airbus-seclab.github.io/c-compiler-security/
    add_compile_options("/W4")

endif()



# Allow the linker to remove unused data and functions
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  add_compile_options("-fdata-sections")
  add_compile_options("-ffunction-sections")
  add_compile_options("-fno-common")
  add_compile_options("-Wl,--gc-sections")
endif()
