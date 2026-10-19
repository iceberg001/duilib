project(NodeDemo)

FILE(GLOB LOCAL_SRC_FILES "*.cpp")

link_libraries(DuiLib)

add_executable(NodeDemo ${LOCAL_SRC_FILES})

add_custom_command(TARGET NodeDemo POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${PROJECT_SOURCE_DIR}/NodeDemoRes $<TARGET_FILE_DIR:NodeDemo>/NodeDemoRes)