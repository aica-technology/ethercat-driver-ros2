#include <gtest/gtest.h>
#include <memory>

#include <pluginlib/class_loader.hpp>
#include "ethercat_interface/ec_slave.hpp"

TEST(TestLoadBota_FTSensor, load_ec_module)
{
  pluginlib::ClassLoader<ethercat_interface::EcSlave> ec_loader_{
    "ethercat_interface", "ethercat_interface::EcSlave"};
  ASSERT_NO_THROW(ec_loader_.createSharedInstance("ethercat_plugins/Bota_FTSensor"));
}

