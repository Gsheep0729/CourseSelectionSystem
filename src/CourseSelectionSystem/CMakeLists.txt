cmake_minimum_required(VERSION 4.1.1)

# https://github.com/Kitware/CMake/blob/v4.1.1/Help/dev/experimental.rst
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
set(CMAKE_CXX_MODULE_STD ON)

project(CourseSelectionSystem LANGUAGES CXX)

find_package(PkgConfig REQUIRED)

# 引入数据库核心库 (Traditional C++ Static Lib)
add_subdirectory(../lib_db_core lib_db_core)

# 1. 定义各层级的模块文件列表 (Code Grouping)
add_executable(CourseSelectionSystem
    main.cpp


)

# 表现层模块 (Presentation Layer)
set(PRESENTATION_MODULES
    # CourseSelectionSystem/presentation/.cppm
    # presentation/pre.cppm
    presentation/pres.cli.cppm
)

# 应用逻辑层模块 (Application Layer)
set(APPLICATION_MODULES
    # CourseSelectionSystem/application/.cppm
    application/app.controller.cppm
)

# 领域层模块 (Domain Layer)
set(DOMAIN_MODULES
    # CourseSelectionSystem/domain/.cppm
    domain/dom.course.cppm
    domain/dom.student.cppm
    domain/domain.cppm
    domain/dom.teacher.cppm
    domain/dom.timeslot.cppm
)

# 数据管理层模块 (Infrastructure Layer)
set(INFRASTRUCTURE_MODULES
    # CourseSelectionSystem/infrastructure/.cppm
    infrastructure/infra.db_adapter.cppm
    infrastructure/infra.course_proxy.cppm
    infrastructure/infra.student_proxy.cppm
    infrastructure/infra.enrollment_proxy.cppm
    infrastructure/infrastructure.cppm
)

# 模块实现文件 (Module Implementation Units)
set(MODULE_IMPL_SOURCES
    infrastructure/infra.db_adapter.cpp
)

# 2. 配置 IDE 中的显示分组 (IDE Display Grouping)
source_group("Presentation" FILES ${PRESENTATION_MODULES})
source_group("Application"  FILES ${APPLICATION_MODULES})
source_group("Domain"       FILES ${DOMAIN_MODULES})
source_group("Infrastructure" FILES ${INFRASTRUCTURE_MODULES} ${MODULE_IMPL_SOURCES})

# 3. 构建目标与源文件关联

# 添加普通源文件 (包括模块实现单元)
target_sources(CourseSelectionSystem PRIVATE
    ${MODULE_IMPL_SOURCES}
)

# 添加模块接口文件 (定义 Module Interface Units)
target_sources(CourseSelectionSystem PRIVATE FILE_SET cxx_modules TYPE CXX_MODULES
    BASE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}
    FILES
        course_system.cppm
        ${PRESENTATION_MODULES}
        ${APPLICATION_MODULES}
        ${DOMAIN_MODULES}
        ${INFRASTRUCTURE_MODULES}
)

target_compile_features(CourseSelectionSystem PRIVATE cxx_std_23)


target_link_libraries(CourseSelectionSystem PRIVATE db_core)












































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

