cmake_minimum_required(VERSION 3.8)
project(ethercat_bota_modules)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(ament_cmake_ros REQUIRED)
find_package(ethercat_interface REQUIRED)
find_package(pluginlib REQUIRED)

file(GLOB_RECURSE PLUGINS_SRC src/*.cpp)
add_library(${PROJECT_NAME} ${PLUGINS_SRC})
target_compile_features(${PROJECT_NAME} PUBLIC c_std_99 cxx_std_17)  # Require C99 and C++17
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
ament_target_dependencies(
  ${PROJECT_NAME}
  "ethercat_interface"
  "pluginlib"
)
pluginlib_export_plugin_description_file(ethercat_interface ethercat_plugins.xml)

install(
  TARGETS ${PROJECT_NAME}
  EXPORT export_${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  find_package(pluginlib REQUIRED)
  find_package(ethercat_interface REQUIRED)

  ament_lint_auto_find_test_dependencies()

  ament_add_gmock(
    test_load_bota_plugins
    test/test_load_ec_modules.cpp
  )
  target_include_directories(test_load_bota_plugins PRIVATE include)
  ament_target_dependencies(test_load_bota_plugins
    pluginlib
    ethercat_interface
  )
endif()
ament_export_include_directories(
  include
)
ament_export_libraries(
  ${PROJECT_NAME}
)
ament_export_targets(
  export_${PROJECT_NAME}
)
ament_package()
