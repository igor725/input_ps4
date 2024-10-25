cmake_minimum_required(VERSION 3.24)

set(PKG_TITLE "PS4 Test Input System")
set(PKG_VERSION "1.2")
set(PKG_TITLE_ID "DDRM00001")
set(PKG_CONTENT_ID "IV0000-${PKG_TITLE_ID}_00-PS4INPUTSY000000")

project(eboot VERSION 0.0.1)
set(PRJ_ADD_LIBS SceVideoOut SceAudioOut ScePad SceUserService)

add_subdirectory(input)

add_executable(eboot
  $<TARGET_OBJECTS:eboot_obj>
  ${OO_PS4_TOOLCHAIN}/lib/crt1.o
)

set_target_properties(eboot PROPERTIES OUTPUT_NAME "eboot" SUFFIX ".elf" PREFIX "")
target_link_libraries(eboot PRIVATE ${PRJ_ADD_LIBS})

# Create param.sfo

if(CMAKE_HOST_WIN32)
  install(CODE "execute_process(WORKING_DIRECTORY \"${PROJECT_SOURCE_DIR}\" COMMAND \".\\\\package.bat\" \"${PKG_TITLE}\" \"${PKG_VERSION}\" \"${PKG_TITLE_ID}\" \"${PKG_CONTENT_ID}\")")
  set(ORBIS_BIN_PATH ${OO_PS4_TOOLCHAIN}/bin/windows)
elseif(CMAKE_HOST_LINUX)
  install(CODE "execute_process(WORKING_DIRECTORY \"${PROJECT_SOURCE_DIR}\" COMMAND \"./package.sh\" \"${PKG_TITLE}\" \"${PKG_VERSION}\" \"${PKG_TITLE_ID}\" \"${PKG_CONTENT_ID}\")")
  set(ORBIS_BIN_PATH ${OO_PS4_TOOLCHAIN}/bin/linux)
else()
  message(FATAL_ERROR "Unsupported OS")
endif()

# Create eboot.bin from generated elf file

add_custom_command(TARGET eboot POST_BUILD COMMAND ${ORBIS_BIN_PATH}/create-fself -in "${CMAKE_BINARY_DIR}/eboot.elf" --out "${CMAKE_BINARY_DIR}/eboot.oelf" --eboot "${PROJECT_SOURCE_DIR}/eboot.bin" --paid 0x3800000000000011)
