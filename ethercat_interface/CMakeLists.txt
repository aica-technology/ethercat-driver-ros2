cmake_minimum_required(VERSION 3.8)
project(ethercat_interface)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(ament_cmake_ros REQUIRED)
find_package(rclcpp REQUIRED)

# EtherLab
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(ETHERLAB_FLAGS
  --disable-8139too --disable-eoe --enable-generic --disable-kernel --with-systemdsystemunitdir=no
  CACHE STRING "EtherLab configuration flags"
)
message(STATUS "EtherLab configuration flags: ${ETHERLAB_FLAGS}")
include(ExternalProject)
set(ETHERLAB_SRC_DIR "${CMAKE_BINARY_DIR}/etherlab-src")
set(ETHERLAB_DST_DIR "${CMAKE_BINARY_DIR}/etherlab")
ExternalProject_Add(
  etherlab_project
  GIT_REPOSITORY https://gitlab.com/etherlab.org/ethercat.git
  GIT_TAG stable-1.5
  GIT_SUBMODULES "" # Disable submodules
  SOURCE_DIR ${ETHERLAB_SRC_DIR}
  BUILD_IN_SOURCE ON
  CONFIGURE_COMMAND ./bootstrap && ./configure ${ETHERLAB_FLAGS} --prefix=${ETHERLAB_DST_DIR}
  BUILD_COMMAND make
  INSTALL_COMMAND make install
)
set(ETHERLAB_INCLUDE_DIRS "${ETHERLAB_SRC_DIR}/include")
set(ETHERLAB_LIBRARY_DIRS "${ETHERLAB_DST_DIR}/lib")
set(ETHERCAT_LIBRARY etherlab)

add_library(etherlab SHARED IMPORTED)
add_dependencies(etherlab etherlab_project)
set_target_properties(etherlab PROPERTIES IMPORTED_LOCATION "${ETHERLAB_LIBRARY_DIRS}/libethercat.so")
install(
  DIRECTORY ${ETHERLAB_INCLUDE_DIRS}/
  DESTINATION etherlab/include
)
install(
  DIRECTORY ${ETHERLAB_LIBRARY_DIRS}/
  DESTINATION etherlab/lib
)
ament_export_include_directories(
  include
  ${ETHERLAB_INCLUDE_DIRS}
)

add_library(
  ${PROJECT_NAME}
  SHARED
  src/ec_master.cpp)
add_dependencies(${PROJECT_NAME} etherlab_project)

target_include_directories(
  ${PROJECT_NAME}
  PRIVATE
  include
  ${ETHERLAB_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} ${ETHERCAT_LIBRARY})

ament_target_dependencies(
  ${PROJECT_NAME}
  rclcpp
)

# INSTALL
install(
  TARGETS ${PROJECT_NAME}
  DESTINATION lib
)
install(
  DIRECTORY include/
  DESTINATION include
)

if(BUILD_TESTING)
  # find_package(ament_cmake_gtest REQUIRED)
  find_package(ament_lint_auto REQUIRED)
  find_package(yaml_cpp_vendor REQUIRED)
  ament_lint_auto_find_test_dependencies()

  # Test PdoChannelManager
  ament_add_gmock(
    test_ec_pdo_channel_manager
    test/test_ec_pdo_channel_manager.cpp
  )
  add_dependencies(test_ec_pdo_channel_manager etherlab_project)
  target_include_directories(test_ec_pdo_channel_manager PRIVATE include ${ETHERLAB_INCLUDE_DIRS})
  ament_target_dependencies(test_ec_pdo_channel_manager
    yaml_cpp_vendor
  )
endif()

## EXPORTS
ament_export_include_directories(
  include
)
ament_export_libraries(
  ${PROJECT_NAME}
  ${ETHERCAT_LIBRARY}
)
ament_export_dependencies(
  rclcpp
)
ament_package()
