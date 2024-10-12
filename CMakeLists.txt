cmake_minimum_required(VERSION 3.14)

project(WDBx VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets REQUIRED)

find_package(WDBReader CONFIG REQUIRED)
find_package(StormLib CONFIG REQUIRED)
find_package(CascLib CONFIG REQUIRED)
find_package(unofficial-tree-sitter CONFIG REQUIRED)

add_subdirectory(src)

set(PROJECT_SOURCES
        src/database.cpp
        src/database.h
        src/databasemodel.cpp
        src/databasemodel.h
        src/exportdialog.cpp
        src/exportdialog.h
        src/exportdialog.ui
        src/main.cpp
        src/mainwindow.cpp
        src/mainwindow.h
        src/mainwindow.ui
        src/opendialog.cpp
        src/opendialog.h
        src/opendialog.ui
        src/filesystem.cpp
        src/filesystem.h
        src/schemaexport.cpp
        src/schemaexport.h
        src/schemahelpers.h
        src/settings.cpp
        src/settings.h
        src/table.cpp
        src/table.h
        src/table.ui
        src/queryeditor.cpp
        src/queryeditor.h
        src/queryeditor.ui
        src/query.h
        src/tree_sitter/parser.c
        src/tree_sitter/parser.h
        src/tree_sitter/array.h
        src/tree_sitter/alloc.h
        src/tree_sitter/tree-sitter-WDBx.h
)

qt_add_executable(WDBx
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    src/WDBx.rc
)

target_link_libraries(WDBx PRIVATE Qt6::Widgets)

set_target_properties(WDBx PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_compile_definitions(WDBx PRIVATE CASCLIB_NO_AUTO_LINK_LIBRARY)
# if(BUILD_SHARED_LIBS)
    # target_link_libraries(WDBx PRIVATE CascLib::casc)
# else()
    target_link_libraries(WDBx PRIVATE CascLib::casc_static)
# endif()

target_compile_definitions(WDBx PRIVATE STORMLIB_NO_AUTO_LINK)
target_link_libraries(WDBx PRIVATE StormLib::storm)

target_link_libraries(WDBx PRIVATE WDBReader::WDBReader)
target_link_libraries(WDBx PRIVATE unofficial::tree-sitter::tree-sitter)

if(WIN32)
    target_link_libraries(WDBx PRIVATE propsys.lib)
endif()

target_compile_definitions(WDBx PUBLIC WDBX_VERSION="${CMAKE_PROJECT_VERSION}")

install(TARGETS WDBx
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

qt_finalize_executable(WDBx)
