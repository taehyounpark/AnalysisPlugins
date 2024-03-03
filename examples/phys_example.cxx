#include <chrono>
#include <iostream>
#include <memory>
#include <sstream>

#include "TFile.h"
#include "TH1F.h"
#include "TLorentzVector.h"
#include "TPad.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TVector2.h"
#include <ROOT/RVec.hxx>

#include "queryosity/queryosity.h"

#include "AnalysisPlugins/Event.h"
#include "AnalysisPlugins/Folder.h"
#include "AnalysisPlugins/Hist.h"

#include <xAODEventInfo/EventInfo.h>
#include <xAODMuon/MuonContainer.h>

using VecF = ROOT::RVec<float>;
using VecD = ROOT::RVec<double>;
using TLV = TLorentzVector;

using cut = queryosity::selection::cut;
using weight = queryosity::selection::weight;

int main() {

  auto df = queryosity::dataflow<Event>(
      {"/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/ASG/DAOD_PHYS/p5169/"
       "mc20_13TeV.410470.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.deriv.DAOD_"
       "PHYS.e6337_s3681_r13167_p5169/DAOD_PHYS.29445530._000001.pool.root.1"},
      "CollectionTree");
  auto eventInfo = df.read<xAOD::EventInfo>("EventInfo");
  auto allMuons = df.read<xAOD::MuonContainer>("Muons");

  class MuonSelection
      : public queryosity::column::definition<
            ConstDataVector<xAOD::MuonContainer>(xAOD::MuonContainer)> {
  public:
    MuonSelection(double etaMax) : m_etaMax(etaMax) {}
    virtual ~MuonSelection() = default;
    virtual ConstDataVector<xAOD::MuonContainer>
    evaluate(queryosity::column::observable<xAOD::MuonContainer> muons)
        const override {
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

  auto selMuons = df.define<MuonSelection>(1.5)(allMuons);
  auto selMuonsPtMeV =
      df.define([](ConstDataVector<xAOD::MuonContainer> const &muons) {
        VecD pts;
        for (const xAOD::Muon *mu : muons) {
          pts.push_back(mu->pt());
        }
        return pts;
      })(selMuons);
  auto selMuonsPt = selMuonsPtMeV / df.constant(1000.);

  auto mcEventWeight = df.define([](const xAOD::EventInfo &eventInfo) {
    return eventInfo.mcEventWeight();
  })(eventInfo);
  auto mcEventWeighted = df.filter<weight>("mcEventWeight")(mcEventWeight);

  auto selMuonsPtHist = df.agg<Hist<1, VecF>>("muons_pt", 100, 0, 100)
                            .fill(selMuonsPt)
                            .at(mcEventWeighted);

  selMuonsPtHist->Draw();
  gPad->Print("muons_pt.pdf");

  return 0;
}