#pragma once

#include <memory>
#include <unordered_map>
#include <utility>

#include "ana/analogical.h"

#include "TObject.h"
#include "TDirectory.h"
#include "TFile.h"

class Folder : public ana::aggregation::summary<Folder>
{

public:
  static TDirectory* subDirectory(TDirectory& dir, const std::string& path)
  {
    // check if directory exists
    auto subDir = dir.GetDirectory(path.c_str());
    // if it doesn't, make one
    if (!subDir) {
      dir.mkdir(path.c_str(), path.c_str(), true);
      subDir = dir.GetDirectory(path.c_str());
    }
    // return either the existing or newly-created subdirectory
    return subDir;
  }

public:
  Folder(const std::string& basePath) : m_basePath(basePath) {}
  ~Folder() = default;

  void record(const std::string& variationName, const std::string& selectionPath, std::shared_ptr<TObject> aggregationResult)
  {
    if (m_variedResults.find(variationName)==m_variedResults.end()) {
      m_variedResults.emplace(std::make_pair(variationName,std::unordered_map<std::string,std::shared_ptr<TObject>>({std::make_pair(selectionPath,aggregationResult)})));
    } else {
      m_variedResults[variationName].emplace(std::make_pair(selectionPath,aggregationResult));
    }
  }

  void record(const std::string& selectionPath, std::shared_ptr<TObject> aggregationResult)
  {
    m_nominalResults.emplace(std::make_pair(selectionPath,aggregationResult));
  }

  void output(TFile& outputFile)
  {
    // nominal results
    for (auto const& [selPath, nomRes] : m_nominalResults) {
      auto nomFolder = this->subDirectory(outputFile, m_basePath);
      auto selFolder = this->subDirectory(*nomFolder, selPath);
      selFolder->WriteObject(nomRes->Clone(), nomRes->GetName());
    }
    // varied results
    for (auto const& [varName, varMap] : m_variedResults) {
      auto varFolder = this->subDirectory(outputFile, m_basePath+"_"+varName);
      for (auto const& [selPath, varRes] : m_variedResults.at(varName)) {
        auto selFolder = this->subDirectory(*varFolder, selPath);
        selFolder->WriteObject(varRes->Clone(), varRes->GetName());
      }
    }
  }

public:
  std::string m_basePath;
  std::unordered_map<std::string, std::shared_ptr<TObject>> m_nominalResults;
  std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<TObject>>> m_variedResults;
  
};