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
  class Loop;

  template <typename T> class Container;

public:
  Event(const std::vector<std::string> &inputFiles,
        const std::string &collection = "CollectionTree",
        const std::string &metadata = "MetaData");
  ~Event() = default;

  queryosity::dataset::partition parallelize();
  double normalize();

  std::unique_ptr<Loop> read(unsigned int) const;

  template <typename U>
  std::unique_ptr<Container<U>> read(unsigned int,
                                     const std::string &name) const;

protected:
  std::vector<std::string> m_inputFiles;
  std::string m_treeName;
  std::string m_metaName;

  std::vector<std::string> m_goodFiles;
};

class Event::Loop : public queryosity::dataset::player {
public:
  Loop(TTree *tree);
  ~Loop() = default;

  virtual void start(const queryosity::dataset::range &part) override;
  virtual void next(const queryosity::dataset::range &part,
                    unsigned long long entry) override;
  virtual void finish(const queryosity::dataset::range &part) override;

protected:
  std::unique_ptr<xAOD::TEvent> m_event;
  long long m_current;
  long long m_end;
};

template <typename T>
class Event::Container : public queryosity::column::reader<T> {

public:
  Container(const std::string &containerName, xAOD::TEvent &event)
      : m_containerName(containerName), m_event(&event) {}
  ~Container() = default;

  virtual T const &read(unsigned int, unsigned long long entry) const override {
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
Event::Loop::read(const queryosity::dataset::range &,
                  const std::string &containerName) const {
  return std::make_unique<Container<U>>(containerName, *m_event);
}
