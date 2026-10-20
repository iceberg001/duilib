# DuiLib extension controls library
# Static library, same as DuiLib on MSVC
project(DuiLibExtension)

# Include directories: main project headers + extension headers
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Cross-platform sources
set(EXT_SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/UIAnimation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/UIFadeButton.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/UITimeButton.cpp
)

# Windows-only sources (GDI+ / system controls / tray icon)
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    set(EXT_SRC_FILES ${EXT_SRC_FILES}
        ${CMAKE_CURRENT_SOURCE_DIR}/UIElipsePhoto.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/UIHotKey.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/UIIPAddress.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/UIColorPalette.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/UIDateTime.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/UITrayIcon.cpp
    )
endif()

# Static library (consistent with DuiLib on MSVC)
add_library(DuiLibExtension STATIC ${EXT_SRC_FILES})

# Link to DuiLib + zlibstatic (transitive deps)
target_link_libraries(DuiLibExtension PUBLIC DuiLib)

# Windows: link GDI+ for UIElipsePhoto
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        target_link_libraries(DuiLibExtension PUBLIC gdiplus)
    else()
        target_link_libraries(DuiLibExtension PUBLIC gdiplus)
    endif()
endif()

# Propagate include dir to consumers
target_include_directories(DuiLibExtension PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})