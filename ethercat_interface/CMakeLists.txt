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
include(ExternalProject)
option(ETHERLAB_FLAGS "EtherLab configuration flags" "--disable-8139too --disable-eoe --enable-generic --disable-kernel --with-systemdsystemunitdir=no")
option(ETHERLAB_DIR "Where to install EtherLab" "${CMAKE_INSTALL_PREFIX}")
ExternalProject_Add(
  etherlab
  GIT_REPOSITORY https://gitlab.com/etherlab.org/ethercat.git
  GIT_TAG stable-1.5
  GIT_SUBMODULES "" # Disable submodules
  BUILD_IN_SOURCE ON
  CONFIGURE_COMMAND ./bootstrap && ./configure ${ETHERLAB_FLAGS} --prefix=${ETHERLAB_DIR}
  BUILD_COMMAND make
  INSTALL_COMMAND make install
)
set(ETHERCAT_LIBRARY ${ETHERLAB_DIR}/lib/libethercat.so)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

ament_export_include_directories(
  include
  ${ETHERLAB_DIR}/include
)

add_library(
  ${PROJECT_NAME}
  SHARED
  src/ec_master.cpp)

target_include_directories(
  ${PROJECT_NAME}
  PRIVATE
  include
  ${ETHERLAB_DIR}/include
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
  target_include_directories(test_ec_pdo_channel_manager PRIVATE include ${ETHERLAB_DIR}/include)
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
