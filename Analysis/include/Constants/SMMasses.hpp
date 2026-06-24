// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

namespace HNL::Constants {
///@name Standard Model particle masses
///@{
inline constexpr double massUp{0.00216};         ///< Up quark mass (2.16 MeV)
inline constexpr double massDown{0.00467};       ///< Down quark mass (4.67 MeV)
                                                 //
inline constexpr double massStrange{0.0934};     ///< Strange quark mass (93.4 MeV)
inline constexpr double massCharm{1.27};         ///< Charm quark mass (1.27 GeV)
                                                 //
inline constexpr double massBottom{4.18};        ///< Bottom quark mass (4.18 GeV)
inline constexpr double massTopDirect{172.69};   ///< Top quark mass [direct] (172.69 GeV)
inline constexpr double massTopCross{162.5};     ///< Top quark mass [cross] (162.5 GeV)
inline constexpr double massTopPole{172.5};      ///< Top quark mass [pole] (162.5 GeV)
inline constexpr double massTop{massTopDirect};  ///< Top quark mass [pole] (162.5 GeV)

inline constexpr double massElectron{0.00051099895000};  ///< Electron mass (511 keV)
inline constexpr double massMuon{0.1056583755};          ///< Muon mass (106 MeV)
inline constexpr double massTau{1.77686};                ///< Tau mass (1.77 GeV)

inline constexpr double massW{80.377};      ///< W boson mass (80.4 GeV)
inline constexpr double massZ{91.1876};     ///< Z boson mass (91.2 GeV)
inline constexpr double massHiggs{125.25};  ///< Higgs boson mass (125.2 GeV)
///@}
}  // namespace HNL::Constants
