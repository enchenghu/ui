set(COMMON_INCLUDE_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../include")
file(GLOB COMMON_INCLUDE
    "${COMMON_INCLUDE_DIRECTORY}/*.h"
)
set(OUT_LIB_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/lib/${CMAKE_BUILD_TYPE})
set(OUT_BIN_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin/${CMAKE_BUILD_TYPE})
file(GLOB OUT_BINS
    "${OUT_BIN_DIRECTORY}/*.dll"
)

# 通用编译选项 引入vcpkg并指定triplet
macro(set_common_build_option)
    set(VCPKG_TARGET_TRIPLET x64-windows)
    set(CMAKE_TOOLCHAIN_FILE $ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/${CMAKE_BUILD_TYPE})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/${CMAKE_BUILD_TYPE})
endmacro(set_common_build_option)

# 可执行文件编译选项
macro(set_exe_build_option)
    set_common_build_option()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})
endmacro(set_exe_build_option)

# 库编译选项
macro(set_dll_build_option)
    set_common_build_option()
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUT_LIB_DIRECTORY})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUT_LIB_DIRECTORY})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUT_BIN_DIRECTORY})
endmacro(set_dll_build_option)

# 自动拷贝config文件至运行路径下
function(copy_file_to_exe_dir COPY_FILE_PATH)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${COPY_FILE_PATH}
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endfunction(copy_file_to_exe_dir)

# cmake通用选项
macro(set_cmake_option)
    set(CMAKE_INCLUDE_CURRENT_DIR ON)

    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)

    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endmacro(set_cmake_option)

# 添加Qt组件 多个组件用分号分隔并用双引号包围 如 "Core;Widgets"
function(find_qt_components ARG_QT_COMPONENTS)
    find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS ${ARG_QT_COMPONENTS})
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${ARG_QT_COMPONENTS})
endfunction(find_qt_components)

# 自动添加可执行文件项目路径下的源文件 其他源文件可通过参数引入 参数可以使用分号分隔并用双引号包围 也可以传入list
function(auto_add_executable ARG_OTHER_SOURCES)
    file(GLOB PROJECT_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.ui"
    )
    list(APPEND PROJECT_SOURCES ${ARG_OTHER_SOURCES})
    include_directories(${COMMON_INCLUDE_DIRECTORY})
    link_directories(${OUT_LIB_DIRECTORY})
    add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
endfunction(auto_add_executable)

# 自动添加库项目路径下的源文件 其他源文件可通过参数引入 参数可以使用分号分隔并用双引号包围 也可以传入list
function(auto_add_library ARG_OTHER_SOURCES)
    file(GLOB PROJECT_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.json"
    )
    list(APPEND PROJECT_SOURCES ${ARG_OTHER_SOURCES})
    include_directories(${COMMON_INCLUDE_DIRECTORY})
    link_directories(${OUT_LIB_DIRECTORY})
    add_library(${PROJECT_NAME} SHARED
        ${PROJECT_SOURCES}
        ${OUT_COMMON_INCLUDE}
    )
endfunction(auto_add_library)

function(deployqt)
    # 获取 Qt bin 目录
    get_filename_component(QT_BIN_DIR "${QT_QMAKE_EXECUTABLE}" DIRECTORY)
    
    # 查找 windeployqt
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt
        PATHS "${QT_BIN_DIR}"
        REQUIRED
        NO_DEFAULT_PATH
    )
    
    # 添加部署命令
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND "${WINDEPLOYQT_EXECUTABLE}"
            --verbose 1
            "$<TARGET_FILE:${PROJECT_NAME}>"
        COMMENT "Deploying Qt libraries using windeployqt for compilation target '${PROJECT_NAME}' ..."
    )
    
    message(STATUS "Qt deployment configured: ${WINDEPLOYQT_EXECUTABLE}")
endfunction()
