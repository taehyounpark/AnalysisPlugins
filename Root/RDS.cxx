#include "AnalysisPlugins/RDS.h"

#include "TROOT.h"

RDS::RDS(std::unique_ptr<RDataSource> rds) : m_rds(std::move(rds)) {}

ana::dataset::partition RDS::parallelize() {
  // force multithreading
  ROOT::EnableImplicitMT();
  m_rds->SetNSlots(ROOT::GetThreadPoolSize() ? ROOT::GetThreadPoolSize() : 1);

  // get allocated slots
  auto slots = m_rds->GetEntryRanges();
  ana::dataset::partition parts;
  for (size_t islot = 0; islot < slots.size(); ++islot) {
    parts.emplace_back(islot, slots[islot].first, slots[islot].second);
  }

  // use whatever ROOT has decided
  parts.fixed = true;
  return parts;
}

void RDS::initialize() { m_rds->Initialize(); }

void RDS::finalize() { m_rds->Finalize(); }

std::unique_ptr<RDS::Reader> RDS::open(const ana::dataset::range &) const {
  return std::make_unique<Reader>(*m_rds);
}

RDS::Reader::Reader(RDataSource &rds) : m_rds(&rds) {}

void RDS::Reader::initialize(const ana::dataset::range &part) {
  m_rds->InitSlot(part.slot, part.begin);
}

void RDS::Reader::execute(const ana::dataset::range &part,
                          unsigned long long entry) {
  m_rds->SetEntry(part.slot, entry);
}

void RDS::Reader::finalize(const ana::dataset::range &part) {
  m_rds->FinalizeSlot(part.slot);
}