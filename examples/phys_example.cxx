#include "AnalysisPlugins/Event.h"
#include "AnalysisPlugins/Hist.h"

#include <xAODEventInfo/EventInfo.h>
#include <xAODMuon/MuonContainer.h>

using VecF = ROOT::RVec<float>;
using VecD = ROOT::RVec<double>;
using TLV = TLorentzVector;

#include "queryosity/queryosity.h"

using dataflow = queryosity::dataflow;
namespace multithread = queryosity::multithread;
namespace dataset = queryosity::dataset;
namespace column = queryosity::column;
namespace query = queryosity::query;
namespace systematic = queryosity::systematic;

#include "TFile.h"
#include "TH1F.h"
#include "TLorentzVector.h"
#include "TPad.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TVector2.h"
#include <ROOT/RVec.hxx>

#include <chrono>
#include <iostream>
#include <memory>
#include <sstream>

class MuonSelection
    : public column::definition<ConstDataVector<xAOD::MuonContainer>(
          xAOD::MuonContainer)> {
public:
  MuonSelection(double etaMax) : m_etaMax(etaMax) {}
  virtual ~MuonSelection() = default;
  virtual ConstDataVector<xAOD::MuonContainer>
  evaluate(column::observable<xAOD::MuonContainer> muons) const override {
    ConstDataVector<xAOD::MuonContainer> selMuons(SG::VIEW_ELEMENTS);
    for (const xAOD::Muon *mu : *muons) {
      if (TMath::Abs(mu->eta()) < 1.5) {
        selMuons.push_back(mu);
      }
    }
    return selMuons;
  }

protected:
  double m_etaMax;
};

int main() {

  dataflow df;

  std::vector<std::string> evnt_files{
      "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/ASG/DAOD_PHYS/p5169/"
      "mc20_13TeV.410470.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.deriv.DAOD_"
      "PHYS.e6337_s3681_r13167_p5169/DAOD_PHYS.29445530._000001.pool.root.1"};
  auto ds = df.load(dataset::input<Event>(evnt_files, "CollectionTree"));

  auto eventInfo = ds.read(dataset::column<xAOD::EventInfo>("EventInfo"));
  auto allMuons = ds.read(dataset::column<xAOD::MuonContainer>("Muons"));
  auto selMuons = df.define(column::definition<MuonSelection>(1.5),allMuons);

  auto getPts = [](ConstDataVector<xAOD::MuonContainer> const &muons) {
    VecD pts; pts.reserve(muons.size());
    for (const xAOD::Muon *mu : muons) {
      pts.push_back(mu->pt());
    }
    return pts;
  };
  auto selMuonsPtMeV =
      df.define(column::expression(getPts),selMuons);

  auto toGeV = df.define(column::constant(1.0/1000.0));
  auto selMuonsPt = selMuonsPtMeV * toGeV;

  auto mcEventWeight = df.define(
    column::expression([](const xAOD::EventInfo &eventInfo) {
    return eventInfo.mcEventWeight();
    }),eventInfo
    );
  auto mcEventWeighted = df.weight(mcEventWeight);

  auto selMuonsPtHist = df.make(query::plan<Hist<1, VecF>>("muons_pt", 100, 0, 100))
                            .fill(selMuonsPt)
                            .book(mcEventWeighted);

  selMuonsPtHist->Draw();
  gPad->Print("muons_pt.pdf");

  return 0;
}