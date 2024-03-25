#include "qhep/Event.h"

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

void Event::parallelize(unsigned int nslots) {
  m_tevents.resize(nslots);
  for (unsigned int islot = 0; islot < nslots; ++islot) {
    auto tree = std::make_unique<TChain>(m_treeName.c_str(), m_treeName.c_str());
    for (auto const &filePath : m_inputFiles) {
      tree->Add(filePath.c_str());
    }
    tree->ResetBit(kMustCleanup);
    auto tevent = std::make_unique<xAOD::TEvent>();
    if (tevent->readFrom(tree.release()).isFailure()) {
      throw std::runtime_error("failed to read event");
    };
    m_tevents[islot] = std::move(tevent);
  }
}

std::vector<std::pair<unsigned long long, unsigned long long>> Event::partition() {
  TDirectory::TContext c;

  std::vector<std::pair<unsigned long long, unsigned long long>> parts;
  
  // check all files for tree clusters
  long long offset = 0;
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
    parts.emplace_back(offset, offset + fileEntries);
    offset += fileEntries;
  }

  return parts;
}

void Event::initialize(unsigned int, unsigned long long, unsigned long long) {
  // nothing to do
}

void Event::execute(unsigned int slot, unsigned long long entry) {
  if (m_tevents[slot]->getEntry(entry) < 0) {
    throw std::runtime_error("failed to get entry");
  }
}

void Event::finalize(unsigned int) {
  // nothing to do
}