// Copyright 2023-2026 Bruno M. S. Oliveira

// Standard Library
#include <format>
#include <string>

// HNL
#include "MCPID/SeesawICurrents.hpp"
#include "Models/SeesawICurrents.hpp"

namespace HNL::Model {
SeesawICurrents::SeesawICurrents(const MG5_aMC::SLHAReader &slhaReader)
    : Generic{ModelID::HeavyN_Majorana} {
  mn1 = slhaReader.get_block_entry("mass", PID::SeesawICurrents::n1);
  mn2 = slhaReader.get_block_entry("mass", PID::SeesawICurrents::n2);
  mn3 = slhaReader.get_block_entry("mass", PID::SeesawICurrents::n3);

  ve1 = slhaReader.get_block_entry("neutrinos", 1);
  ve2 = slhaReader.get_block_entry("neutrinos", 2);
  ve3 = slhaReader.get_block_entry("neutrinos", 3);

  vm1 = slhaReader.get_block_entry("neutrinos", 4);
  vm2 = slhaReader.get_block_entry("neutrinos", 5);
  vm3 = slhaReader.get_block_entry("neutrinos", 6);

  vt1 = slhaReader.get_block_entry("neutrinos", 7);
  vt2 = slhaReader.get_block_entry("neutrinos", 8);
  vt3 = slhaReader.get_block_entry("neutrinos", 9);

  n1Width = slhaReader.get_block_entry("decay", PID::SeesawICurrents::n1);
  n2Width = slhaReader.get_block_entry("decay", PID::SeesawICurrents::n2);
  n3Width = slhaReader.get_block_entry("decay", PID::SeesawICurrents::n3);
};

SeesawICurrents::SeesawICurrents(const std::vector<double> &parametersVector)
    : Generic{ModelID::HeavyN_Majorana} {
  mn1 = parametersVector[0];
  ve1 = parametersVector[1];
  vm1 = parametersVector[2];
  vt1 = parametersVector[3];
  n1Width = parametersVector[4];

  mn2 = parametersVector[5];
  ve2 = parametersVector[6];
  vm2 = parametersVector[7];
  vt2 = parametersVector[8];
  n2Width = parametersVector[9];

  mn2 = parametersVector[10];
  ve2 = parametersVector[11];
  vm2 = parametersVector[12];
  vt2 = parametersVector[13];
  n2Width = parametersVector[14];
};

std::string SeesawICurrents::CSV() const {
  return std::format("{},{},{},{},{},{},{},{},{},{},{},{},{},{},{}", mn1, ve1, vm1, vt1, n1Width,
                     mn2, ve2, vm2, vt2, n2Width, mn3, ve3, vm3, vt3, n3Width);
}

std::vector<double> SeesawICurrents::Vector() const {
  return {mn1, ve1, vm1, vt1, n1Width, mn2, ve2, vm2, vt2, n2Width, mn3, ve3, vm3, vt3, n3Width};
}

double SeesawICurrents::Theta1Squared() const { return (ve1 * ve1) + (vm1 * vm1) + (vt1 * vt1); }

double SeesawICurrents::Theta2Squared() const { return (ve2 * ve2) + (vm2 * vm2) + (vt2 * vt2); }

double SeesawICurrents::Theta3Squared() const { return (ve3 * ve3) + (vm3 * vm3) + (vt3 * vt3); }
}  // namespace HNL::Model
