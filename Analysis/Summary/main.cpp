// Copyright 2023-2026 Bruno M. S. Oliveira

// Standard Library
#include <cstddef>
#include <print>
#include <string>

// CLI11
#include "CLI/CLI.hpp"

// HNL
#include "DelphesWrapper/ReaderParticle.hpp"
#include "MCPID/SPSS.hpp"
#include "MG5_aMC/MadgraphData.hpp"

namespace {
#define PRINT_EVENTS FALSE
inline void LogEvent([[maybe_unused]] const size_t id, [[maybe_unused]] const HepMCEvent &event) {
#if PRINT_EVENTS
  std::println("{},{},{},{}", id, event.CrossSection, event.CrossSectionError, event.Weight);
#endif
}

#define PRINT_DECAYS FALSE
inline void LogDecay([[maybe_unused]] const HNL::MG5::Particle &n1,
                     [[maybe_unused]] const HNL::MG5::Particle &n2) {
#if PRINT_DECAYS
  std::println("\n{}\n{}", n1.ToString(true), n2.ToString(true));
#endif
}

}  // namespace

using DelphesReader = HNL::Delphes::FileReaderParticle;

int main(const int argc, const char *const argv[]) {
  /* ----- Command Line Arguments ----- */
  CLI::App app{};

  std::string study{};
  app.add_option("--study", study, "Study name")->required();

  std::string simulation{};
  app.add_option("--name", simulation, "Simulation name")->required();

  int runId{};
  app.add_option("--run", runId, "run")->required();

  int tagId{};
  app.add_option("--tag", tagId, "tag")->required();

  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError &e) {
    return app.exit(e);
  }

  /* ----- Event Reconstruction ----- */
  HNL::MG5::MadgraphData madgraphData{study, simulation, runId, tagId};

  DelphesReader delphesReader{madgraphData.eventsPath};
  const int64_t nEventsSimulated{delphesReader.GetEntries()};

  for (size_t idx{0}; delphesReader.Next(); ++idx) {
    for (const HepMCEvent &event : delphesReader.Event) {
      LogEvent(idx, event);
    }
  }

  /* ----- Output ----- */
  std::print("{},", madgraphData.CSV());
  std::print("{}", nEventsSimulated);

  if (madgraphData.model->ID() == HNL::Model::ModelID::SPSS) {
    const auto n1{madgraphData.FetchDecayModes(HNL::PID::SPSS::n1)};
    const auto n2{madgraphData.FetchDecayModes(HNL::PID::SPSS::n2)};

    LogDecay(n1, n2);
  }

  std::println();

  return 0;
}
