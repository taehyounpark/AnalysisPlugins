#include "qhep/Event.h"
#include "qhep/Hist.h"

#include <xAODEventInfo/EventInfo.h>
#include <xAODEgamma/ElectronContainer.h>

using VecF = ROOT::RVec<float>;
using VecD = ROOT::RVec<double>;

#include "queryosity/queryosity.h"

using dataflow = queryosity::dataflow;
namespace multithread = queryosity::multithread;
namespace dataset = queryosity::dataset;
namespace column = queryosity::column;
namespace query = queryosity::query;
namespace systematic = queryosity::systematic;

#include "TH1F.h"
#include "TPad.h"
#include "TCanvas.h"
#include <ROOT/RVec.hxx>

#include <chrono>
#include <iostream>
#include <memory>
#include <sstream>

std::vector<std::string> daodFiles{
"/project/6001378/thpark/public/mc23_13p6TeV.601189.PhPy8EG_AZNLO_Zee.deriv.DAOD_PHYS.e8514_s4162_r14622_p5855/DAOD_PHYS.35010014._000001.pool.root.1",
"/project/6001378/thpark/public/mc23_13p6TeV.601189.PhPy8EG_AZNLO_Zee.deriv.DAOD_PHYS.e8514_s4162_r14622_p5855/DAOD_PHYS.35010014._000002.pool.root.1",
"/project/6001378/thpark/public/mc23_13p6TeV.601189.PhPy8EG_AZNLO_Zee.deriv.DAOD_PHYS.e8514_s4162_r14622_p5855/DAOD_PHYS.35010014._000003.pool.root.1",
"/project/6001378/thpark/public/mc23_13p6TeV.601189.PhPy8EG_AZNLO_Zee.deriv.DAOD_PHYS.e8514_s4162_r14622_p5855/DAOD_PHYS.35010014._000004.pool.root.1",
"/project/6001378/thpark/public/mc23_13p6TeV.601189.PhPy8EG_AZNLO_Zee.deriv.DAOD_PHYS.e8514_s4162_r14622_p5855/DAOD_PHYS.35010014._000005.pool.root.1",
"/project/6001378/thpark/public/mc23_13p6TeV.601189.PhPy8EG_AZNLO_Zee.deriv.DAOD_PHYS.e8514_s4162_r14622_p5855/DAOD_PHYS.35010014._000006.pool.root.1",
"/project/6001378/thpark/public/mc23_13p6TeV.601189.PhPy8EG_AZNLO_Zee.deriv.DAOD_PHYS.e8514_s4162_r14622_p5855/DAOD_PHYS.35010014._000007.pool.root.1",
"/project/6001378/thpark/public/mc23_13p6TeV.601189.PhPy8EG_AZNLO_Zee.deriv.DAOD_PHYS.e8514_s4162_r14622_p5855/DAOD_PHYS.35010014._000008.pool.root.1",
"/project/6001378/thpark/public/mc23_13p6TeV.601189.PhPy8EG_AZNLO_Zee.deriv.DAOD_PHYS.e8514_s4162_r14622_p5855/DAOD_PHYS.35010014._000009.pool.root.1",
"/project/6001378/thpark/public/mc23_13p6TeV.601189.PhPy8EG_AZNLO_Zee.deriv.DAOD_PHYS.e8514_s4162_r14622_p5855/DAOD_PHYS.35010014._000010.pool.root.1"
};
std::string treeName = "CollectionTree";

float EventWeight(const xAOD::EventInfo &eventInfo) {
  return eventInfo.mcEventWeight();
}

class ElectronSelection
    : public column::definition<ConstDataVector<xAOD::ElectronContainer>(
          xAOD::ElectronContainer)> {
public:
  ElectronSelection(double pT_min, double eta_max)
      : m_pT_min(pT_min), m_eta_max(eta_max) {}
  virtual ~ElectronSelection() = default;
  virtual ConstDataVector<xAOD::ElectronContainer> evaluate(
      column::observable<xAOD::ElectronContainer> els) const override {
    ConstDataVector<xAOD::ElectronContainer> els_sel(
        SG::VIEW_ELEMENTS);
    for (const xAOD::Electron *el : *els) {
      if (el->pt() < m_pT_min)
        continue;
      if (TMath::Abs(el->eta()) > m_eta_max)
        continue;
      els_sel.push_back(el);
    }
    return els_sel;
  }

protected:
  double m_pT_min;
  double m_eta_max;
};

bool AtLeastTwoElectrons(ConstDataVector<xAOD::ElectronContainer> const &els) {
  return els.size() >= 2;
}

float DiElectronsMass(ConstDataVector<xAOD::ElectronContainer> const &els) {
  return (els[0]->p4() + els[1]->p4()).M();
};

void analyze(unsigned int n) {
  dataflow df(multithread::enable(n));

  auto ds = df.load(dataset::input<Event>(daodFiles, treeName));
  auto eventInfo = ds.read(dataset::column<xAOD::EventInfo>("EventInfo"));
  auto allElectrons =
      ds.read(dataset::column<xAOD::ElectronContainer>("Electrons"));

  auto selectedElectrons =
      df.define(column::definition<ElectronSelection>(10.0,1.5), allElectrons);
  auto diElectronsMassMeV =
      df.define(column::expression(DiElectronsMass), selectedElectrons);
  auto toGeV = df.define(column::constant(1.0 / 1000.0));
  auto diElectronsMassGeV = diElectronsMassMeV * toGeV;

  auto eventWeight = df.define(column::expression(EventWeight), eventInfo);
  auto atLeastTwoSelectedElectrons =
      df.weight(eventWeight)
          .filter(column::expression(AtLeastTwoElectrons), selectedElectrons);

  auto selectedElectronsPtHist =
      df.make(query::plan<Hist<1,float>>("diElectronMass", 100, 0, 500))
          .fill(diElectronsMassGeV)
          .book(atLeastTwoSelectedElectrons);

  selectedElectronsPtHist->Draw();
  gPad->SetLogy();
  gPad->Print("el_pts.pdf");
}

int main(int argc, char *argv[]) { 
  int nthreads = 0;
  if (argc==2) { nthreads=strtol(argv[1], nullptr, 0); }

  auto tic = std::chrono::steady_clock::now();
  analyze(nthreads);
  auto toc = std::chrono::steady_clock::now();
  std::chrono::duration<double> elapsed_seconds = toc-tic;
  std::cout << "elapsed time (" << nthreads << " threads) = " << elapsed_seconds.count() << "s"
            << std::endl;
}