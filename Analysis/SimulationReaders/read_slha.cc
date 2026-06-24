// Copyright 2023-2026 Bruno M. S. Oliveira

// STL
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>

// SimulationReaders
#include "SimulationReaders/read_slha.h"

// HNL
#include "Common/Log.hpp"

namespace HNL::MG5_aMC {
void SLHABlock::set_entry(const std::vector<int>& indices, const double value) {
  if (entries.size() == 0) {
    this->indices = indices.size();
  } else if (this->indices != indices.size()) {
    LogError("Wrong number of indices in set_entry ({} vs {})", this->indices,
             indices.size());
    exit(1);
  }

  entries[indices] = value;
}

double SLHABlock::get_entry(const std::vector<int>& indices,
                            const double default_value) const {
  if (entries.find(indices) == entries.end()) {
    LogWarn("No such entry {} in {}, using default value ({})", indices, name,
            default_value);
    return default_value;
  }

  return entries.at(indices);
}

void SLHAReader::read_slha_file(const std::string_view file_name) {
  if (!std::filesystem::exists(file_name)) {
    LogError("{} does not exist", file_name);
    exit(1);
  }

  std::ifstream param_card;
  param_card.open(file_name.data(), std::ifstream::in);
  if (!param_card.good()) {
    LogError("Error while opening {}", file_name);
    exit(1);
  }

  constexpr size_t buffer_size{200};
  char buffer[buffer_size];
  std::string line;
  std::string block;

  while (param_card.good()) {
    param_card.getline(buffer, buffer_size);
    line = buffer;

    // Change to lowercase
    transform(line.begin(), line.end(), line.begin(),
              static_cast<int (*)(int)>(tolower));
    if (line != "" && line[0] != '#') {
      if (block != "") {
        // Look for double index blocks
        double dindex1{0};
        double dindex2{0};
        double value{0};

        std::stringstream linestr2(line);
        if (linestr2 >> dindex1 >> dindex2 >> value &&
            dindex1 == static_cast<int>(dindex1) &&
            dindex2 == static_cast<int>(dindex2)) {
          const std::vector<int> indices{static_cast<int>(dindex1),
                                         static_cast<int>(dindex2)};
          set_block_entry(block, indices, value);
          continue;
        }

        std::stringstream linestr1(line);
        if (linestr1 >> dindex1 >> value &&
            dindex1 == static_cast<int>(dindex1)) {
          const std::vector<int> indices{static_cast<int>(dindex1)};
          set_block_entry(block, indices, value);
          continue;
        }
      }

      // Look for block
      if (line.find("block ") != std::string::npos) {
        line = line.substr(6);

        // Get rid of spaces between block and block name
        while (line[0] == ' ') line = line.substr(1);

        // Now find end of block name
        size_t space_pos = line.find(' ');
        if (space_pos != std::string::npos) line = line.substr(0, space_pos);

        block = line;

        continue;
      }

      // Look for decay
      if (line.find("decay ") == 0) {
        line = line.substr(6);
        block = "";
        std::stringstream linestr(line);

        int pdg_code{0};
        double value{0};

        if (linestr >> pdg_code >> value) {
          set_block_entry("decay", pdg_code, value);
        } else {
          LogWarn("Wrong format for decay block: {}", line);
        }
        continue;
      }
    }
  }

  if (_blocks.size() == 0) {
    LogError("No information read from SLHA card");
    exit(1);
  }

  param_card.close();
}

double SLHAReader::get_block_entry(const std::string_view block_name,
                                   const std::vector<int>& indices,
                                   const double default_value) const {
  if (_blocks.find(block_name.data()) == _blocks.end()) {
    LogWarn("No such block {}, using default value ({})", block_name,
            default_value);
    return default_value;
  }

  return _blocks.at(block_name.data()).get_entry(indices, default_value);
}

double SLHAReader::get_block_entry(const std::string_view block_name,
                                   const int index,
                                   const double default_value) const {
  const std::vector<int> indices{index};
  return get_block_entry(block_name, indices, default_value);
}

void SLHAReader::set_block_entry(const std::string_view block_name,
                                 const std::vector<int>& indices,
                                 const double value) {
  if (_blocks.find(block_name.data()) == _blocks.end()) {
    SLHABlock block(block_name);
    _blocks[block_name.data()] = block;
  }

  _blocks[block_name.data()].set_entry(indices, value);
}

void SLHAReader::set_block_entry(const std::string_view block_name,
                                 const int index, const double value) {
  const std::vector<int> indices{index};
  set_block_entry(block_name, indices, value);
}
}  // namespace HNL::MG5_aMC
