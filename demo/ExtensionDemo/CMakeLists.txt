project(ExtensionDemo)

FILE(GLOB LOCAL_SRC_FILES "*.cpp")

link_libraries(DuiLibExtension)

add_executable(ExtensionDemo ${LOCAL_SRC_FILES})

add_custom_command(TARGET ExtensionDemo POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${PROJECT_SOURCE_DIR}/ExtensionDemoRes $<TARGET_FILE_DIR:ExtensionDemo>/ExtensionDemoRes)