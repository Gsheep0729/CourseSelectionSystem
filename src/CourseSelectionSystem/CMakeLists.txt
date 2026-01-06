cmake_minimum_required(VERSION 4.1.1)

# https://github.com/Kitware/CMake/blob/v4.1.1/Help/dev/experimental.rst
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
set(CMAKE_CXX_MODULE_STD ON)

project(CourseSelectionSystem LANGUAGES CXX)


# 1. 配置依赖库
find_package(PkgConfig REQUIRED)          # 加载 PkgConfig 模块
pkg_check_modules(PQXX REQUIRED libpqxx IMPORTED_TARGET)  # 使用 pkg-config 查找 libpqxx 并定义 PQXX_* 变量



# 2. 定义各层级的模块文件列表 (Code Grouping)
add_executable(CourseSelectionSystem
    main.cpp
)

# 应用逻辑层模块 (Application Layer)
set(APPLICATION_MODULES
    # application/.cppm
    application/app.controller.cppm
)

# 领域层模块 (Domain Layer)
set(DOMAIN_MODULES
    # domain/.cppm
    domain/dom.course.cppm
    domain/dom.student.cppm
    domain/domain.cppm
)

# 数据管理层模块 (Infrastructure Layer)
set(INFRASTRUCTURE_MODULES
    # infrastructure/.cppm
    # infrastructure/infra.db_connection.cppm
    # infrastructure/infra.student_proxy.cppm
)

set(INFRASTRUCTURE_SOURCES
    # infrastructure/infra.db_connection.cpp
    # infrastructure/infra.student_proxy.cpp
)

# 表现层模块 (Presentation Layer)
set(PRESENTATION_MODULES
    # presentation/.cppm
)


# 3. 配置 IDE 中的显示分组 (IDE Display Grouping)
source_group("Presentation Module" FILES ${PRESENTATION_MODULES})
source_group("Application Module"  FILES ${APPLICATION_MODULES})
source_group("Domain Module"       FILES ${DOMAIN_MODULES})
source_group("Infrastructure Module" FILES ${INFRASTRUCTURE_MODULES} ${INFRASTRUCTURE_SOURCES})

# 3. 构建目标与源文件关联
# 将所有模块添加到目标中
target_sources(CourseSelectionSystem PRIVATE
    ${INFRASTRUCTURE_SOURCES}
    PRIVATE
    FILE_SET cxx_modules TYPE CXX_MODULES
    BASE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}
    FILES
        course_system.cppm
        ${PRESENTATION_MODULES}
        ${APPLICATION_MODULES}
        ${DOMAIN_MODULES}
        ${INFRASTRUCTURE_MODULES}
)

# 4. 链接库
# 根据你的环境，可能是 libpqxx::pqxx 或 pqxx
# target_link_libraries(course_system
#     PRIVATE
#         libpqxx::pqxx
# )
target_link_libraries(CourseSelectionSystem
    PRIVATE
    #    PkgConfig::PQXX  # 对应 pkg_check_modules(... IMPORTED_TARGET)
)

target_compile_features(CourseSelectionSystem PRIVATE cxx_std_23)

# 5. 链接库配置 (使用 PkgConfig 变量)
# target_link_libraries(CourseSelectionSystem PRIVATE ${PQXX_LIBRARIES})        # 链接 libpqxx 库
# target_include_directories(CourseSelectionSystem PRIVATE ${PQXX_INCLUDE_DIRS}) # 添加头文件搜索路径
# target_link_directories(CourseSelectionSystem PRIVATE ${PQXX_LIBRARY_DIRS})    # 添加库文件搜索路径











































# target_link_directories(CourseSelectionSystem PRIVATE /opt/utils/lib)  # -L /opt/utils/lib 设置链接路径
# target_link_libraries(CourseSelectionSystem PRIVATE utils)             # -lutils           设置链接库名

# set_target_properties(CourseSelectionSystem PROPERTIES
#     INSTALL_RPATH_USE_LINK_PATH  True                           # 使用链接路径作为安装文件的RPath，等价于"-Wl,-rpath=/opt/utils/lib"
# )

#add_library(CourseSelectionSystem SHARED
#  utils.cpp
#)

#set_target_properties( CourseSelectionSystem PROPERTIES
#    PUBLIC_HEADER utils.cppm
#    VERSION   2.5.1
#    SOVERSION 2
#    LIBRARY_OUTPUT_DIRECTORY  "${CMAKE_BINARY_DIR}/lib"
#)

#install(TARGETS CourseSelectionSystem
#    LIBRARY DESTINATION /opt/utils/lib
#    PUBLIC_HEADER DESTINATION  /opt/utils/modules
#)

#add_custom_command(TARGET CourseSelectionSystem
#    POST_BUILD
#    COMMAND cmake --install "${CMAKE_CURRENT_BINARY_DIR}"
#)
