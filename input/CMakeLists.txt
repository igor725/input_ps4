project(eboot_obj LANGUAGES CXX)

add_library(eboot_obj OBJECT
  # Entry
  main.cpp
  controller.cpp
  graphics.cpp
  png.cpp
)

target_include_directories(eboot_obj PRIVATE ../padpatch)
