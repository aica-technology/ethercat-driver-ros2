#include <cstring>

#include "ethercat_interface/ec_slave.hpp"
#include "ethercat_plugins/commondefs.hpp"

namespace ethercat_plugins {

class SensOneFtSensor : public ethercat_interface::EcSlave {
public:
  SensOneFtSensor() : EcSlave(0x0000b07a, 0x00000001) {}
  virtual ~SensOneFtSensor() {}
  virtual void processData(size_t index, uint8_t* domain_address) {
    if (data_index_[index] >= 0) {
      uint32_t value = EC_READ_S32(domain_address);
      float ret;
      memcpy(&ret, &value, sizeof(float));
      state_interface_ptr_->at(data_index_[index]) = static_cast<double>(ret);
    }
  }
  virtual const ec_sync_info_t* syncs() { return &syncs_[0]; }
  virtual size_t syncSize() {
    return sizeof(syncs_) / sizeof(ec_sync_info_t);
  }
  virtual const ec_pdo_entry_info_t* channels() {
    return channels_;
  }
  virtual void domains(DomainMap& domains) const {
    domains = domains_;
  }
  virtual bool setupSlave(
      std::unordered_map<std::string, std::string> slave_paramters, std::vector<double>* state_interface,
      std::vector<double>* command_interface
  ) {
    state_interface_ptr_ = state_interface;
    command_interface_ptr_ = command_interface;
    paramters_ = slave_paramters;

    if (paramters_.find("state_interface/force.x") != paramters_.end()) {
      data_index_[1] = std::stoi(paramters_["state_interface/force.x"]);
    }
    if (paramters_.find("state_interface/force.y") != paramters_.end()) {
      data_index_[2] = std::stoi(paramters_["state_interface/force.y"]);
    }
    if (paramters_.find("state_interface/force.z") != paramters_.end()) {
      data_index_[3] = std::stoi(paramters_["state_interface/force.z"]);
    }
    if (paramters_.find("state_interface/torque.x") != paramters_.end()) {
      data_index_[4] = std::stoi(paramters_["state_interface/torque.x"]);
    }
    if (paramters_.find("state_interface/torque.y") != paramters_.end()) {
      data_index_[5] = std::stoi(paramters_["state_interface/torque.y"]);
    }
    if (paramters_.find("state_interface/torque.z") != paramters_.end()) {
      data_index_[6] = std::stoi(paramters_["state_interface/torque.z"]);
    }
    return true;
  }

private:
  int data_index_[7] = {-1, -1, -1, -1, -1, -1, -1};

  ec_pdo_entry_info_t channels_[23] = {{0x7000, 0x00, 8}, /* Digital Output */
                                       {0x6000, 0x01, 8}, /* Status */
                                       {0x6000, 0x02, 32}, /* Warnings Errors Fatals */
                                       {0x6000, 0x04, 32}, /* Force x */
                                       {0x6000, 0x05, 32}, /* Force y */
                                       {0x6000, 0x06, 32}, /* Force z */
                                       {0x6000, 0x07, 32}, /* Torque x */
                                       {0x6000, 0x08, 32}, /* Torque y */
                                       {0x6000, 0x09, 32}, /* Torque z */
                                       {0x6000, 0x0a, 16}, /* Force Torque Saturated */
                                       {0x6000, 0x0b, 32}, /* Acceleration x */
                                       {0x6000, 0x0c, 32}, /* Acceleration y */
                                       {0x6000, 0x0d, 32}, /* Acceleration z */
                                       {0x6000, 0x0e, 8}, /* Acceleration Saturated */
                                       {0x6000, 0x0f, 32}, /* Angular Rate x */
                                       {0x6000, 0x10, 32}, /* Angular Rate y */
                                       {0x6000, 0x11, 32}, /* Angular Rate z */
                                       {0x6000, 0x12, 8}, /* Angular Rate Saturated */
                                       {0x6000, 0x13, 32}, /* Temperature */
                                       {0x6000, 0x14, 32}, /* Orientation Estimate x */
                                       {0x6000, 0x15, 32}, /* Orientation Estimate y */
                                       {0x6000, 0x16, 32}, /* Orientation Estimate z */
                                       {0x6000, 0x17, 32}, /* Orientation Estimate w */
  };

  ec_pdo_info_t pdos_[2] = {{0x1600, 1, channels_ + 0}, /* Digital Output */
                            {0x1a00, 22, channels_ + 1}, /* Sensor Data */
  };

  ec_sync_info_t syncs_[5] = {{0, EC_DIR_OUTPUT, 0, NULL, EC_WD_DISABLE}, {1, EC_DIR_INPUT, 0, NULL, EC_WD_DISABLE},
                              {2, EC_DIR_OUTPUT, 1, pdos_ + 0, EC_WD_DISABLE},
                              {3, EC_DIR_INPUT, 1, pdos_ + 1, EC_WD_DISABLE}, {0xff}};

  DomainMap domains_ = {{0, {0, 3, 4, 5, 6, 7, 8}}};
};

}  // namespace ethercat_plugins

#include <pluginlib/class_list_macros.hpp>

PLUGINLIB_EXPORT_CLASS(ethercat_plugins::SensOneFtSensor, ethercat_interface::EcSlave)
