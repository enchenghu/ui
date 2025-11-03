cmake_minimum_required(VERSION 3.14)
include(../CMakeCommon/Common.cmake)

add_definitions(-D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set_exe_build_option()
    project(M1DatabaseOrm LANGUAGES CXX)
    set_cmake_option()
    set(DB_FILE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/db/M1.sqlite)
else()
    set(DB_FILE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/db/M1.sqlite PARENT_SCOPE)
endif()

function(copy_db_to_exe_dir)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${DB_FILE_PATH}
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endfunction(copy_db_to_exe_dir)


find_package(SqliteOrm GLOBAL REQUIRED)
find_package(sqlcipher GLOBAL REQUIRED)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    auto_add_executable("test/main.cpp")
    target_link_libraries(${PROJECT_NAME} PRIVATE sqlite_orm::sqlite_orm sqlcipher::sqlcipher)
    copy_db_to_exe_dir()
else()
    file(GLOB DATABASEORM_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    )
    set(DATABASEORM_SRC ${DATABASEORM_SRC} PARENT_SCOPE)
endif()
