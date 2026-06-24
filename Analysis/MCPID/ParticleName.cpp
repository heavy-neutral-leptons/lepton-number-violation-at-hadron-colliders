// Copyright 2023-2026 Bruno M. S. Oliveira

// HNL
#include "MCPID/ParticleName.hpp"

#include "MCPID/SM.hpp"

namespace HNL::PID {
std::string ParticleName(const int pid) {
  switch (std::abs(pid)) {
      // Quarks
    case SM::down:
      return "d";
    case SM::up:
      return "u";
    case SM::strange:
      return "strange";
    case SM::charm:
      return "c";
    case SM::bottom:
      return "b";
    case SM::top:
      return "t";

      // Leptons
    case SM::electron:
      return "e";
    case SM::velectron:
      return "νe";
    case SM::muon:
      return "µ";
    case SM::vmuon:
      return "νµ";
    case SM::tau:
      return "τ";
    case SM::vtau:
      return "ντ";

      // Bosons
    case SM::gluon:
      return "g";
    case SM::gamma:
      return "γ";
    case SM::z:
      return "Z";
    case SM::w:
      return "W";
    case SM::h:
      return "H";

    default:
      return std::to_string(pid);
  }
}
}  // namespace HNL::PID
