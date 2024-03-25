#pragma once

#include <memory>
#include <string>
#include <vector>

#include "TChain.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TString.h"
#include "TTree.h"

#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"

#include "queryosity/queryosity.h"

class Event : public queryosity::dataset::reader<Event> {

public:
  template <typename T> class Container;

public:
  Event(const std::vector<std::string> &inputFiles,
        const std::string &collection = "CollectionTree",
        const std::string &metadata = "MetaData");
  ~Event() = default;

  virtual void parallelize(unsigned int) override;
  virtual std::vector<std::pair<unsigned long long, unsigned long long>> partition() override;
  virtual void initialize(unsigned int, unsigned long long, unsigned long long) override;
  virtual void execute(unsigned int, unsigned long long) override;
  virtual void finalize(unsigned int) override;

  template <typename U>
  std::unique_ptr<Container<U>> read(unsigned int slot,
                                     const std::string &name) const;

protected:
  std::vector<std::string> m_inputFiles;
  std::string m_treeName;
  std::string m_metaName;

  std::vector<std::unique_ptr<xAOD::TEvent>> m_tevents;
};

template <typename T>
class Event::Container : public queryosity::column::reader<T> {

public:
  Container(const std::string &containerName, xAOD::TEvent &event)
      : m_containerName(containerName), m_event(&event) {}
  ~Container() = default;

  virtual T const &read(unsigned int, unsigned long long) const override {
    T const *container(nullptr);
    if (m_event->retrieve(container, this->m_containerName.c_str())
            .isFailure()) {
      throw std::runtime_error(TString::Format(
          "failed to retrieve '%s' from event", this->m_containerName.c_str()));
    }
    return *container;
  }

protected:
  std::string m_containerName;
  xAOD::TEvent *m_event;
};

template <typename U>
std::unique_ptr<Event::Container<U>>
Event::read(unsigned int slot,
                  const std::string &containerName) const {
  return std::make_unique<Container<U>>(containerName, *m_tevents[slot]);
}
