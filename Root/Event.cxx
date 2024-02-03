#include "AnalysisPlugins/Event.h"

#include "TROOT.h"

#include "xAODCutFlow/CutBookkeeper.h"
#include "xAODCutFlow/CutBookkeeperAuxContainer.h"
#include "xAODCutFlow/CutBookkeeperContainer.h"

Event::Event(const std::vector<std::string> &inputFiles,
             const std::string &collection, const std::string &metadata)
    : m_inputFiles(inputFiles), m_treeName(collection), m_metaName(metadata) {
  ROOT::EnableThreadSafety();
  xAOD::Init().ignore();
}

double Event::normalize() {
  return 1.0; // can (should?) normalize xAOD::CutBookkeeper sumOfWeights
}

ana::dataset::partition Event::parallelize() {
  TDirectory::TContext c;

  ana::dataset::partition parts;
  unsigned int ipart = 0;
  long long offset = 0;

  // check all files for tree clusters
  for (const auto &filePath : m_inputFiles) {

    // check file
    auto file = std::unique_ptr<TFile>(TFile::Open(filePath.c_str()));
    if (!file) {
      continue;
    } else if (file->IsZombie()) {
      continue;
    }

    // check tree
    auto tree = file->Get<TTree>(m_treeName.c_str());
    auto event = std::make_unique<xAOD::TEvent>();
    if (!tree || event->readFrom(tree).isFailure()) {
      continue;
    }

    // get file entries
    long long fileEntries = event->getEntries();
    // add part to parts
    m_goodFiles.push_back(filePath);
    parts.add_part(ipart++, offset, offset + fileEntries);
    offset += fileEntries;
  }

  return parts;
}

std::unique_ptr<Event::Loop> Event::read(const ana::dataset::range &) const {
  auto tree = std::make_unique<TChain>(m_treeName.c_str(), m_treeName.c_str());
  for (auto const &filePath : m_goodFiles) {
    tree->Add(filePath.c_str());
  }
  tree->ResetBit(kMustCleanup);
  return std::make_unique<Loop>(tree.release());
}

Event::Loop::Loop(TTree *tree) {
  m_event = std::make_unique<xAOD::TEvent>();
  if (m_event->readFrom(tree).isFailure()) {
    throw std::runtime_error("failed to read event");
  };
}

void Event::Loop::start(const ana::dataset::range &) {
  // nothing to do
}

void Event::Loop::next(const ana::dataset::range &, unsigned long long entry) {
  if (m_event->getEntry(entry) < 0) {
    throw std::runtime_error("failed to get entry");
  }
}

void Event::Loop::finish(const ana::dataset::range &) {
  // nothing to do
}