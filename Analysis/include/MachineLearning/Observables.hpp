// Copyright 2023-2026 Bruno M. S. Oliveira

#pragma once

// Standard Library
#include <filesystem>
#include <vector>

// HNL
#include "Common/Observable.hpp"

namespace HNL::ML {
/**
 * Observable set used for prompt lepton-number-violating analyses.
 *
 * The branch names in this list must match the branch names present in the ROOT trees used for TMVA
 * training and application.
 */
namespace DimuonSS {
const std::vector<Common::Observable> observables{
    // {.name = "l1_qe", .title = "Q(#ell_1)", .unit = ""},
    {.name = "l1_pt", .title = "P_T(#ell_1)", .unit = "GeV"},
    {.name = "l1_et", .title = "#eta(#ell_1)", .unit = ""},
    // {.name = "l1_ph", .title = "#phi(#ell_1)", .unit = ""},

    // {.name = "l2_qe", .title = "Q(#ell_2)", .unit = ""},
    {.name = "l2_pt", .title = "P_T(#ell_2)", .unit = "GeV"},
    {.name = "l2_et", .title = "#eta(#ell_2)", .unit = ""},
    // {.name = "l2_ph", .title = "#phi(#ell_2)", .unit = ""},

    {.name = "j1_pt", .title = "P_T(j_1)", .unit = "GeV"},
    {.name = "j1_et", .title = "#eta(j_1)", .unit = ""},
    // {.name = "j1_ph", .title = "#phi(j_1)", .unit = ""},
    {.name = "j1_ma", .title = "m(j_1)", .unit = "GeV"},

    {.name = "j2_pt", .title = "P_T(j_2)", .unit = "GeV"},
    {.name = "j2_et", .title = "#eta(j_2)", .unit = ""},
    // {.name = "j2_ph", .title = "#phi(j_2)", .unit = ""},
    {.name = "j2_ma", .title = "m(j_2)", .unit = "GeV"},

    {.name = "j12_pt", .title = "P_T(j_1+j_2)", .unit = "GeV"},
    {.name = "j12_et", .title = "#eta(j_1+j_2)", .unit = ""},
    // {.name = "j12_ph", .title = "#phi(j_1+j_2)", .unit = ""},
    {.name = "j12_ma", .title = "m(j_1+j_2)", .unit = "GeV"},

    {.name = "ms_et", .title = "E_T(Miss)", .unit = "GeV"},

    {.name = "l1_l2_dr", .title = "#Delta R(#ell_1, #ell_2)", .unit = ""},
    {.name = "l1_l2_de", .title = "#Delta #eta(#ell_1, #ell_2)", .unit = ""},
    {.name = "l1_l2_dp", .title = "#Delta #phi(#ell_1, #ell_2)", .unit = ""},

    {.name = "l1_j1_dr", .title = "#Delta R(#ell_1, j_1)", .unit = ""},
    {.name = "l1_j1_de", .title = "#Delta #eta(#ell_1, j_1)", .unit = ""},
    {.name = "l1_j1_dp", .title = "#Delta #phi(#ell_1, j_1)", .unit = ""},
    {.name = "l1_j2_dr", .title = "#Delta R(#ell_1, j_2)", .unit = ""},
    {.name = "l1_j2_de", .title = "#Delta #eta(#ell_1, j_2)", .unit = ""},
    {.name = "l1_j2_dp", .title = "#Delta #phi(#ell_1, j_2)", .unit = ""},
    {.name = "l1_j12_dr", .title = "#Delta R(#ell_1, j_1 + j_2)", .unit = ""},
    {.name = "l1_j12_de", .title = "#Delta #eta(#ell_1, j_1 + j_2)", .unit = ""},
    {.name = "l1_j12_dp", .title = "#Delta #phi(#ell_1, j_1 + j_2)", .unit = ""},

    {.name = "l2_j1_dr", .title = "#Delta R(#ell_2, j_1)", .unit = ""},
    {.name = "l2_j1_de", .title = "#Delta #eta(#ell_2, j_1)", .unit = ""},
    {.name = "l2_j1_dp", .title = "#Delta #phi(#ell_2, j_1)", .unit = ""},
    {.name = "l2_j2_dr", .title = "#Delta R(#ell_2, j_2)", .unit = ""},
    {.name = "l2_j2_de", .title = "#Delta #eta(#ell_2, j_2)", .unit = ""},
    {.name = "l2_j2_dp", .title = "#Delta #phi(#ell_2, j_2)", .unit = ""},
    {.name = "l2_j12_dr", .title = "#Delta R(#ell_2, j_1 + j_2)", .unit = ""},
    {.name = "l2_j12_de", .title = "#Delta #eta(#ell_2, j_1 + j_2)", .unit = ""},
    {.name = "l2_j12_dp", .title = "#Delta #phi(#ell_2, j_1 + j_2)", .unit = ""},

    {.name = "j1_j2_dr", .title = "#Delta R(j_1, j_2)", .unit = ""},
    {.name = "j1_j2_de", .title = "#Delta #eta(j_1, j_2)", .unit = ""},
    {.name = "j1_j2_dp", .title = "#Delta #phi(j_1, j_2)", .unit = ""},
};
};  // namespace DimuonSS

/**
 * Observable set used for purely trimuon analyses.
 *
 * The branch names in this list must match the branch names present in the ROOT trees used for TMVA
 * training and application.
 */
namespace Trimuon {
const std::vector<Common::Observable> observables{
    // {.name = "l1_qe", .title = "Q(#ell_1)", .unit = ""},
    {.name = "l1_pt", .title = "P_T(#ell_1)", .unit = "GeV"},
    {.name = "l1_et", .title = "#eta(#ell_1)", .unit = ""},
    // {.name = "l1_ph", .title = "#phi(#ell_1)", .unit = ""},

    // {.name = "l2_qe", .title = "Q(#ell_2)", .unit = ""},
    {.name = "l2_pt", .title = "P_T(#ell_2)", .unit = "GeV"},
    {.name = "l2_et", .title = "#eta(#ell_2)", .unit = ""},
    // {.name = "l2_ph", .title = "#phi(#ell_2)", .unit = ""},

    // {.name = "l3_qe", .title = "Q(#ell_3)", .unit = ""},
    {.name = "l3_pt", .title = "P_T(#ell_3)", .unit = "GeV"},
    {.name = "l3_et", .title = "#eta(#ell_3)", .unit = ""},
    // {.name = "l3_ph", .title = "#phi(#ell_3)", .unit = ""},

    {.name = "ms_et", .title = "E_T(Miss)", .unit = "GeV"},

    {.name = "l1_l2_dr", .title = "#Delta R(#ell_1, #ell_2)", .unit = ""},
    {.name = "l1_l2_de", .title = "#Delta #eta(#ell_1, #ell_2)", .unit = ""},
    {.name = "l1_l2_dp", .title = "#Delta #phi(#ell_1, #ell_2)", .unit = ""},

    {.name = "l1_l3_dr", .title = "#Delta R(#ell_1, #ell_3)", .unit = ""},
    {.name = "l1_l3_de", .title = "#Delta #eta(#ell_1, #ell_3)", .unit = ""},
    {.name = "l1_l3_dp", .title = "#Delta #phi(#ell_1, #ell_3)", .unit = ""},

    {.name = "l2_l3_dr", .title = "#Delta R(#ell_2, #ell_3)", .unit = ""},
    {.name = "l2_l3_de", .title = "#Delta #eta(#ell_2, #ell_3)", .unit = ""},
    {.name = "l2_l3_dp", .title = "#Delta #phi(#ell_2, #ell_3)", .unit = ""},
};
};  // namespace Trimuon

/**
 * Select the observable list associated with a trained dataset path.
 *
 * The current implementation infers the analysis channel from the dataset path string. Paths
 * containing `DimuonSS` map to `DimuonSS::observables` and paths containing `Trimuon` map to
 * `Trimuon::observables`.
 *
 * @param path Dataset path used by the TMVA tools.
 * @return Pointer to the observable set matching the dataset path.
 */
const std::vector<Common::Observable>* Observables(const std::filesystem::path& path);
}  // namespace HNL::ML
