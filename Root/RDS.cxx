#include "AnalysisPlugins/RDS.h"

#include "TROOT.h"

RDS::RDS(std::unique_ptr<RDataSource> rds) : m_rds(std::move(rds)) {}

void RDS::parallelize(unsigned int nslots) { m_rds->SetNSlots(nslots); }

std::vector<std::pair<unsigned long long, unsigned long long>>
RDS::partition() {
  return m_rds->GetEntryRanges();
}

void RDS::initialize() { m_rds->Initialize(); }

void RDS::finalize() { m_rds->Finalize(); }

void RDS::initialize(unsigned int slot, unsigned long long begin,
                     unsigned long long /* end */) {
  m_rds->InitSlot(slot, begin);
}

void RDS::execute(unsigned int slot, unsigned long long entry) {
  m_rds->SetEntry(slot, entry);
}

void RDS::finalize(unsigned int slot) { m_rds->FinalizeSlot(slot); }