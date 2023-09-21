#include <memory>
#include <chrono>
#include <iostream>
#include <sstream>
#include <functional>

#include <ROOT/RVec.hxx>
#include "Math/Vector4D.h"
#include "TPad.h"
#include "TFile.h"
#include "TTreeReaderValue.h"
#include "TTreeReader.h"
#include "TVector2.h"
#include "TH1F.h"

#include "ana/analogical.h"

#include "AnalysisPlugins/Tree.h"
#include "AnalysisPlugins/Hist.h"
#include "AnalysisPlugins/Folder.h"

using cut = ana::selection::cut;
using weight = ana::selection::weight;

using VecF = ROOT::RVec<float>;
using VecD = ROOT::RVec<double>;
using VecI = ROOT::RVec<int>;
using VecUI = ROOT::RVec<unsigned int>;
using P4 = ROOT::Math::PtEtaPhiEVector;

class NthP4 : public ana::column::definition<P4(VecD, VecD, VecD, VecD)>
{

public:
  NthP4(unsigned int index) : 
    m_index(index)
  {}
  virtual ~NthP4() = default;

  virtual P4 evaluate(ana::observable<VecD> pt, ana::observable<VecD> eta, ana::observable<VecD> phi, ana::observable<VecD> es) const override {
    return P4(pt->at(m_index),eta->at(m_index),phi->at(m_index),es->at(m_index));
  }

protected:
  const unsigned int m_index;

};
 
int main() {

  ana::multithread::disable();

  ana::dataflow<Tree> df( {"hww.root"}, "mini" );

  auto mc_weight = df.read<float>("mcWeight");
  auto mu_sf = df.read<float>("scaleFactor_MUON");

  auto el_sf = df.read<float>("scaleFactor_ELE");
  // auto el_sf = df.read<float>("scaleFactor_ELE").vary("sf_var","scaleFactor_PILEUP");

  auto lep_pt_MeV = df.read<VecF>("lep_pt");
  auto lep_eta = df.read<VecF>("lep_eta");
  auto lep_phi = df.read<VecF>("lep_phi");
  auto lep_E_MeV = df.read<VecF>("lep_E");
  auto lep_Q = df.read<VecF>("lep_charge");
  auto lep_type = df.read<VecUI>("lep_type");
  auto met_MeV = df.read<float>("met_et");
  auto met_phi = df.read<float>("met_phi");

  auto MeV = df.constant(1000.0);
  auto lep_pt = lep_pt_MeV / MeV;
  auto lep_E = lep_E_MeV / MeV;
  auto met = met_MeV / MeV;

  auto lep_eta_max = df.constant(2.4);

  // auto Escale = df.define([](VecD E){return E;});
  // auto Escale = df.define([](VecD E){return E;}).vary("lp4_up",[](VecD E){return E*1.01;});
  auto Escale = df.define([](VecD E){return E;}).vary("lp4_up",[](VecD E){return E*1.01;}).vary("lp4_dn",[](VecD E){return E*0.99;});

  auto lep_pt_sel = Escale(lep_pt)[ lep_eta < lep_eta_max && lep_eta > (-lep_eta_max) ];
  auto lep_E_sel = Escale(lep_E)[ lep_eta < lep_eta_max && lep_eta > (-lep_eta_max) ];
  auto nlep_sel = df.define([](VecD const& lep){return lep.size();})(lep_pt_sel);

  std::cout << nlep_sel.has_variation("lp4_up") << std::endl;

  auto lep_eta_sel = lep_eta[ lep_eta < lep_eta_max && lep_eta > (-lep_eta_max) ];
  auto lep_phi_sel = lep_phi[ lep_eta < lep_eta_max && lep_eta > (-lep_eta_max) ];

  auto l1p4 = df.define<NthP4>(0)(lep_pt_sel, lep_eta_sel, lep_phi_sel, lep_E_sel);
  auto l2p4 = df.define<NthP4>(1)(lep_pt_sel, lep_eta_sel, lep_phi_sel, lep_E_sel);

  // auto llp4 = l1p4+l2p4;
  auto llp4 = df.define([](const P4& p4, const P4& q4){return p4+q4;})(l1p4, l2p4);
  auto mll = df.define([](const P4& p4){return p4.M();})(llp4);
  auto pth = df.define(
    [](const P4& p4, float q, float q_phi) {
      TVector2 p2; p2.SetMagPhi(p4.Pt(), p4.Phi());
      TVector2 q2; q2.SetMagPhi(q, q_phi);
      return (p2+q2).Mod();
    })(llp4, met, met_phi);

  auto incl = df.filter<weight>("incl")(mc_weight * el_sf * mu_sf);

  auto nlep_req = df.constant<unsigned int>(2);
  auto cut_2los = incl.filter<cut>("2l")(nlep_sel == nlep_req).filter<cut>("2los", [](const VecF& lep_charge){return lep_charge.at(0)+lep_charge.at(1)==0;})(lep_Q);
  auto cut_2ldf = cut_2los.channel<cut>("2ldf", [](const VecI& lep_type){return lep_type.at(0)+lep_type.at(1)==24;})(lep_type);
  auto cut_2lsf = cut_2los.channel<cut>("2lsf", [](const VecI& lep_type){return (lep_type.at(0)+lep_type.at(1)==22)||(lep_type.at(0)+lep_type.at(1)==26);})(lep_type);

  auto mll_cut = df.constant(80.0), met_cut = df.constant(30.0);
  auto cut_2ldf_sr = cut_2ldf.filter<cut>("sr")(mll > mll_cut);
  auto cut_2lsf_sr = cut_2lsf.filter<cut>("sr")(met > met_cut);
  auto cut_2ldf_cr = cut_2ldf.filter<cut>("cr")(mll > mll_cut);
  auto cut_2lsf_cr = cut_2lsf.filter<cut>("cr")(met > met_cut);

  auto pth_hist = df.book<Hist<1,float>>("pth",50,0,400).fill(pth);
  auto pth_2ldf_sr = pth_hist.at(cut_2ldf_sr);
  auto pth_2ldf_cr = pth_hist.at(cut_2ldf_cr);

  auto get_pt = df.define([](P4 const& p4){return p4.Pt();});
  auto l1pt = get_pt(l1p4);
  auto l2pt = get_pt(l2p4);
  auto l1n2pt_hists = df.book<Hist<1,float>>(std::string("l1n2pt"),50,0,200).fill(l1pt).fill(l2pt).at(cut_2los, cut_2lsf, cut_2ldf);
  auto pth_hists = df.book<Hist<1,float>>("pth",50,0,400).fill(pth).at(cut_2los, cut_2ldf, cut_2lsf);

  // pth_hists["2los"]->Draw(); gPad->Print("pth_2los.pdf");

  // // auto out_file = TFile::Open("hww_results.root","recreate");
  // // ana::output::dump<Folder>(pth_hists, *out_file, "ggf");
  // // delete out_file;


  // auto mll_vars_2los = df.book<Hist<1,float>>("mll",50,0,100).fill(mll).at(cut_2los);
  // auto mll_nom_2los = mll_vars_2los.nominal();
  // auto mll_var_2los = mll_vars_2los["lp4_up"];
  // mll_nom_2los->SetLineColor(kBlack); mll_nom_2los->Draw("hist");
  // mll_var_2los->SetLineColor(kRed); mll_var_2los->Draw("E same");
  // gPad->Print("mll_varied_2los.pdf");

  // auto mll_vars_channels = df.book<Hist<1,float>>("mll",50,0,200).fill(mll).at(cut_2ldf, cut_2lsf);
  // std::cout << mll_vars_channels.nominal()["2ldf"]->GetMean() << std::endl;
  // std::cout << mll_vars_channels["lp4_up"]["2lsf"]->GetMean() << std::endl;

  auto miniTree = df.book<Tree::Snapshot<VecD,float,float>>("miniTree","lep_pt_sel","mll","eventWeight").fill(lep_pt_sel,mll,incl).at(cut_2ldf);
  // auto miniTree = df.book<Tree::Snapshot<VecD>>("miniTree","lep_pt_sel").fill(lep_pt_sel).at(cut_2ldf);
  auto outputFile = new TFile("analyzed.root","recreate");
  outputFile->WriteObject(miniTree.nominal().result().get(),miniTree.nominal()->GetName());
  delete outputFile;

  return 0;
}