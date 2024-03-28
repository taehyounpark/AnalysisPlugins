#include "qhep/Hist.h"
#include "qhep/Tree.h"

#include "queryosity/queryosity.h"

namespace qty = queryosity;
using dataflow = qty::dataflow;
namespace multithread = qty::multithread;
namespace dataset = qty::dataset;
namespace column = qty::column;
namespace query = qty::query;
namespace systematic = qty::systematic;

#include "Math/Vector4D.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TStyle.h"
#include "TVector2.h"
#include <ROOT/RVec.hxx>

using VecF = ROOT::RVec<float>;
using VecD = ROOT::RVec<double>;
using VecI = ROOT::RVec<int>;
using VecUI = ROOT::RVec<unsigned int>;
using P4 = ROOT::Math::PtEtaPhiEVector;

#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>

class NthP4 : public column::definition<P4(VecD, VecD, VecD, VecD)> {

public:
  NthP4(unsigned int index) : m_index(index) {}
  virtual ~NthP4() = default;

  virtual P4 evaluate(column::observable<VecD> pt, column::observable<VecD> eta,
                      column::observable<VecD> phi,
                      column::observable<VecD> es) const override {
    return P4(pt->at(m_index), eta->at(m_index), phi->at(m_index),
              es->at(m_index));
  }

protected:
  const unsigned int m_index;
};

int main() {

  // the tree doesn't have enough events to multithread
  dataflow df(multithread::disable());

  std::vector<std::string> tree_files{"hww.root"};
  std::string tree_name = "mini";
  auto ds = df.load(dataset::input<Tree>(tree_files, tree_name));

  // weights
  auto mc_weight = ds.read(dataset::column<float>("mcWeight"));
  auto mu_sf = ds.read(dataset::column<float>("scaleFactor_MUON"));
  auto el_sf = ds.read(dataset::column<float>("scaleFactor_ELE"));

  // leptons
  auto [lep_pt_MeV, lep_eta, lep_phi, lep_E_MeV, lep_Q, lep_type] = ds.read(
      dataset::column<VecF>("lep_pt"), dataset::column<VecF>("lep_eta"),
      dataset::column<VecF>("lep_phi"), dataset::column<VecF>("lep_E"),
      dataset::column<VecF>("lep_charge"), dataset::column<VecUI>("lep_type"));

  // missing transverse energy
  auto [met_MeV, met_phi] = ds.read(dataset::column<float>("met_et"),
                                    dataset::column<float>("met_phi"));

  // units
  auto MeV = df.define(column::constant(1000.0));
  auto lep_pt = lep_pt_MeV / MeV;
  auto lep_E = lep_E_MeV / MeV;
  auto met = met_MeV / MeV;

  // vary the energy scale by +/-2%
  auto Escale =
      df.vary(column::expression([](VecD E) { return E; }),
              systematic::variation("eg_up", [](VecD E) { return E * 1.02; }),
              systematic::variation("eg_dn", [](VecD E) { return E * 0.98; }));

  // apply the energy scale (uncertainties)
  // and select ones within acceptance
  auto lep_pt_min = df.define(column::constant(15.0));
  auto lep_eta_max = df.define(column::constant(2.4));
  auto lep_selection = (lep_eta < lep_eta_max) && (lep_eta > (-lep_eta_max)) &&
                       (lep_pt > lep_pt_min);
  auto lep_pt_sel = Escale(lep_pt)[lep_selection];
  auto lep_E_sel = Escale(lep_E)[lep_selection];
  auto lep_eta_sel = lep_eta[lep_selection];
  auto lep_phi_sel = lep_phi[lep_selection];

  // put (sub-)leading lepton into four-momentum
  auto l1p4 = df.define(column::definition<NthP4>(0), lep_pt_sel, lep_eta_sel,
                        lep_phi_sel, lep_E_sel);
  auto l2p4 = df.define(column::definition<NthP4>(1), lep_pt_sel, lep_eta_sel,
                        lep_phi_sel, lep_E_sel);

  // compute dilepton invariant mass & higgs transverse momentum
  auto llp4 = l1p4 + l2p4;
  auto mll =
      df.define(column::expression([](const P4 &p4) { return p4.M(); }), llp4);
  auto higgs_pt =
      df.define(column::expression([](const P4 &p4, float q, float q_phi) {
                  TVector2 p2;
                  p2.SetMagPhi(p4.Pt(), p4.Phi());
                  TVector2 q2;
                  q2.SetMagPhi(q, q_phi);
                  return (p2 + q2).Mod();
                }),
                llp4, met, met_phi);

  // compute number of leptons
  auto nlep_req = df.define(column::constant<unsigned int>(2));
  auto nlep_sel =
      df.define(column::expression([](VecD const &lep) { return lep.size(); }),
                lep_pt_sel);

  // apply cuts & weights
  auto weighted = df.weight(mc_weight * el_sf * mu_sf);
  auto cut_2l = weighted.filter(nlep_sel == nlep_req);
  auto cut_2los =
      cut_2l.filter(column::expression([](const VecF &lep_charge) {
                      return lep_charge.at(0) + lep_charge.at(1) == 0;
                    }),
                    lep_Q);
  auto cut_2ldf =
      cut_2los.filter(column::expression([](const VecI &lep_type) {
                        return lep_type.at(0) + lep_type.at(1) == 24;
                      }),
                      lep_type);
  auto cut_2lsf =
      cut_2los.filter(column::expression([](const VecI &lep_type) {
                        return (lep_type.at(0) + lep_type.at(1) == 22) ||
                               (lep_type.at(0) + lep_type.at(1) == 26);
                      }),
                      lep_type);
  auto mll_max = df.define(column::constant(80.0));
  auto met_min = df.define(column::constant(30.0));
  auto cut_2ldf_sr = cut_2ldf.filter((mll < mll_max) && (met > met_min));
  auto cut_2lsf_sr = cut_2lsf.filter((mll < mll_max) && (met > met_min));
  auto cut_2los_sr =
      df.filter(cut_2ldf_sr || cut_2lsf_sr).weight(mc_weight * el_sf * mu_sf);
  // once two selections are joined, they "forget" everything upstream
  // i.e. need to re-apply the event weight!

  // histograms:
  // - at three regions
  // - nominal & eg_up & eg_dn
  auto [pth_2los_sr, pth_2ldf_sr, pth_2lsf_sr] =
      df.make(query::plan<Hist<1, float>>("pth", 30, 0, 150))
          .fill(higgs_pt)
          .book(cut_2los_sr, cut_2ldf_sr, cut_2lsf_sr);
  // all done at once :)

  Double_t w = 1600;
  Double_t h = 800;
  TCanvas c("c", "c", w, h);
  c.SetWindowSize(w + (w - c.GetWw()), h + (h - c.GetWh()));
  c.Divide(3, 1);
  c.cd(1);
  pth_2los_sr.nominal()->SetLineColor(kBlack);
  pth_2los_sr.nominal()->Draw("ep");
  pth_2los_sr["eg_up"]->SetLineColor(kRed);
  pth_2los_sr["eg_up"]->Draw("same hist");
  pth_2los_sr["eg_dn"]->SetLineColor(kBlue);
  pth_2los_sr["eg_dn"]->Draw("same hist");
  c.cd(2);
  pth_2ldf_sr.nominal()->SetLineColor(kBlack);
  pth_2ldf_sr.nominal()->Draw("ep");
  pth_2ldf_sr["eg_up"]->SetLineColor(kRed);
  pth_2ldf_sr["eg_up"]->Draw("same hist");
  pth_2ldf_sr["eg_dn"]->SetLineColor(kBlue);
  pth_2ldf_sr["eg_dn"]->Draw("same hist");
  c.cd(3);
  pth_2lsf_sr.nominal()->SetLineColor(kBlack);
  pth_2lsf_sr.nominal()->Draw("ep");
  pth_2lsf_sr["eg_up"]->SetLineColor(kRed);
  pth_2lsf_sr["eg_up"]->Draw("same hist");
  pth_2lsf_sr["eg_dn"]->SetLineColor(kBlue);
  pth_2lsf_sr["eg_dn"]->Draw("same hist");
  c.SaveAs("pth.png");

  return 0;
}