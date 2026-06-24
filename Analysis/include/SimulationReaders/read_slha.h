// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// STL
#include <map>
#include <string>
#include <vector>

namespace HNL::MG5_aMC {
class SLHABlock {
 public:
  explicit SLHABlock(const std::string_view name = "") : name{name} {}
  SLHABlock(const SLHABlock &) = delete;
  SLHABlock(SLHABlock &&) = delete;
  SLHABlock &operator=(const SLHABlock &) = default;
  SLHABlock &operator=(SLHABlock &&) = default;

  ~SLHABlock() = default;

  void set_entry(const std::vector<int> &indices, double value);
  [[nodiscard]] double get_entry(const std::vector<int> &indices,
                                 double default_value = 0) const;

  [[nodiscard]] std::map<std::vector<int>, double> get_entries() const {
    return entries;
  }

  void set_name(const std::string_view name) { this->name = name; }
  [[nodiscard]] std::string_view get_name() const { return name; }

  [[nodiscard]] unsigned int get_indices() const { return indices; }

 private:
  std::string name;

  unsigned int indices{};
  std::map<std::vector<int>, double> entries;
};

class SLHAReader {
 public:
  explicit SLHAReader(const std::string_view file_name = "") {
    if (!file_name.empty()) read_slha_file(file_name);
  }

  void read_slha_file(std::string_view file_name);

  [[nodiscard]] double get_block_entry(std::string_view block_name,
                                       const std::vector<int> &indices,
                                       double default_value = 0) const;
  [[nodiscard]] double get_block_entry(std::string_view block_name, int index,
                                       double default_value = 0) const;

  void set_block_entry(std::string_view block_name,
                       const std::vector<int> &indices, double value);
  void set_block_entry(std::string_view block_name, int index, double value);

 private:
  std::map<std::string, SLHABlock> _blocks;
};
}  // namespace HNL::MG5_aMC
