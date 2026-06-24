// Copyright 2023-2026 Bruno M. S. Oliveira

// Standard Library
#include <format>
#include <string>

// HNL
#include "MCPID/HeavyN.hpp"
#include "Models/HeavyN_Majorana.hpp"

namespace HNL::Model::HeavyN {
Majorana::Majorana(const MG5_aMC::SLHAReader &slhaReader) : Generic{ModelID::HeavyN_Majorana} {
  mn1 = slhaReader.get_block_entry("mass", PID::HeavyN::Majorana::n1);
  mn2 = slhaReader.get_block_entry("mass", PID::HeavyN::Majorana::n2);
  mn3 = slhaReader.get_block_entry("mass", PID::HeavyN::Majorana::n3);

  ven1 = slhaReader.get_block_entry("numixing", 1);
  ven2 = slhaReader.get_block_entry("numixing", 2);
  ven3 = slhaReader.get_block_entry("numixing", 3);

  vmun1 = slhaReader.get_block_entry("numixing", 4);
  vmun2 = slhaReader.get_block_entry("numixing", 5);
  vmun3 = slhaReader.get_block_entry("numixing", 6);

  vtan1 = slhaReader.get_block_entry("numixing", 7);
  vtan2 = slhaReader.get_block_entry("numixing", 8);
  vtan3 = slhaReader.get_block_entry("numixing", 9);

  n1Width = slhaReader.get_block_entry("decay", PID::HeavyN::Majorana::n1);
  n2Width = slhaReader.get_block_entry("decay", PID::HeavyN::Majorana::n2);
  n3Width = slhaReader.get_block_entry("decay", PID::HeavyN::Majorana::n3);
};

Majorana::Majorana(const std::vector<double> &parametersVector)
    : Generic{ModelID::HeavyN_Majorana} {
  mn1 = parametersVector[0];
  ven1 = parametersVector[1];
  vmun1 = parametersVector[2];
  vtan1 = parametersVector[3];
  n1Width = parametersVector[4];

  mn2 = parametersVector[5];
  ven2 = parametersVector[6];
  vmun2 = parametersVector[7];
  vtan2 = parametersVector[8];
  n2Width = parametersVector[9];

  mn2 = parametersVector[10];
  ven2 = parametersVector[11];
  vmun2 = parametersVector[12];
  vtan2 = parametersVector[13];
  n2Width = parametersVector[14];
};

std::string Majorana::CSV() const {
  return std::format("{},{},{},{},{},{},{},{},{},{},{},{},{},{},{}", mn1, ven1, vmun1, vtan1,
                     n1Width, mn2, ven2, vmun2, vtan2, n2Width, mn3, ven3, vmun3, vtan3, n3Width);
}

std::vector<double> Majorana::Vector() const {
  return {mn1,   ven1,    vmun1, vtan1, n1Width, mn2,   ven2,   vmun2,
          vtan2, n2Width, mn3,   ven3,  vmun3,   vtan3, n3Width};
}

double Majorana::Theta1Squared() const { return (ven1 * ven1) + (vmun1 * vmun1) + (vtan1 * vtan1); }

double Majorana::Theta2Squared() const { return (ven2 * ven2) + (vmun2 * vmun2) + (vtan2 * vtan2); }

double Majorana::Theta3Squared() const { return (ven3 * ven3) + (vmun3 * vmun3) + (vtan3 * vtan3); }
}  // namespace HNL::Model::HeavyN
