if(NOT DEFINED ENV{OO_PS4_TOOLCHAIN})
  message(FATAL_ERROR "Missing OpenOrbis toolchain environment variable!")
endif()
STRING(REGEX REPLACE "\\\\" "/" OO_PS4_TOOLCHAIN "$ENV{OO_PS4_TOOLCHAIN}")

set(CMAKE_SYSTEM_NAME FreeBSD)
set(CMAKE_C_COMPILER_TARGET "x86_64-pc-freebsd12-elf")
set(CMAKE_C_FLAGS "-fPIC -funwind-tables -fshort-wchar")
set(CMAKE_CXX_COMPILER_TARGET "${CMAKE_C_COMPILER_TARGET}")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}")
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
set(CMAKE_SYSROOT ${OO_PS4_TOOLCHAIN})

include_directories(SYSTEM
  ${OO_PS4_TOOLCHAIN}/include
  ${OO_PS4_TOOLCHAIN}/include/c++/v1
)

link_directories(
  ${OO_PS4_TOOLCHAIN}/lib
)

add_link_options(-pie -nostartfiles -nodefaultlibs -lc -lc++ -lkernel -fuse-ld=lld -Wl,-m,elf_x86_64 -Wl,--eh-frame-hdr "-Wl,--script,${OO_PS4_TOOLCHAIN}/link.x")

add_compile_options(-nobuiltininc -nostdinc++ -nostdinc)

add_compile_definitions(STBI_NO_SIMD=1)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_options(-O0 -g)
else()
  add_compile_options(-O3)
endif()
