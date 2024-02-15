#pragma once

#include <memory>
#include <string>
#include <vector>

#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include <ROOT/RVec.hxx>

#include "ana/analogical.h"

template <int Dim, typename Prec> class Hist;

template <typename Prec>
class Hist<1, Prec>
    : public ana::counter::definition<std::shared_ptr<TH1>(Prec)> {

public:
  Hist(const std::string &, unsigned int, double, double);
  Hist(const std::string &, const std::vector<double> &);
  virtual ~Hist() = default;

  virtual void fill(ana::observable<Prec>, double) override;
  virtual std::shared_ptr<TH1> result() const override;
  virtual std::shared_ptr<TH1>
  merge(std::vector<std::shared_ptr<TH1>> const &results) const override;

protected:
  // histogram
  std::shared_ptr<TH1> m_hist; //!
};

template <typename Prec>
class Hist<2, Prec>
    : public ana::counter::definition<std::shared_ptr<TH2>(Prec, Prec)> {

public:
  Hist(const std::string &, const std::vector<double> &,
       const std::vector<double> &);
  virtual ~Hist() = default;

  virtual void fill(ana::observable<Prec>, ana::observable<Prec>,
                    double) override;
  virtual std::shared_ptr<TH2> result() const override;
  virtual std::shared_ptr<TH2>
  merge(std::vector<std::shared_ptr<TH2>> const &results) const override;

protected:
  std::shared_ptr<TH2> m_hist; //!
};

template <typename Prec>
class Hist<3, Prec>
    : public ana::counter::definition<std::shared_ptr<TH3>(Prec, Prec, Prec)> {

public:
  Hist(const std::string &, const std::vector<double> &,
       const std::vector<double> &, const std::vector<double> &);
  virtual ~Hist() = default;

  virtual void fill(ana::observable<Prec>, ana::observable<Prec>,
                    ana::observable<Prec>, double) override;
  virtual std::shared_ptr<TH3> result() const override;
  virtual std::shared_ptr<TH3>
  merge(std::vector<std::shared_ptr<TH3>> const &results) const override;

protected:
  std::shared_ptr<TH3> m_hist; //!
};

template <typename Prec>
class Hist<1, ROOT::RVec<Prec>>
    : public ana::counter::definition<std::shared_ptr<TH1>(ROOT::RVec<Prec>)> {

public:
  Hist(const std::string &name, unsigned int nbins, double min, double xmax);
  Hist(const std::string &name, const std::vector<double> &xbins);
  virtual ~Hist() = default;

  virtual void fill(ana::observable<ROOT::RVec<Prec>>, double) override;
  virtual std::shared_ptr<TH1> result() const override;
  virtual std::shared_ptr<TH1>
  merge(std::vector<std::shared_ptr<TH1>> const &results) const override;

protected:
  // histogram
  std::shared_ptr<TH1> m_hist; //!
};

template <typename Prec>
class Hist<2, ROOT::RVec<Prec>>
    : public ana::counter::definition<std::shared_ptr<TH2>(ROOT::RVec<Prec>,
                                                           ROOT::RVec<Prec>)> {

public:
  Hist(const std::string &, const std::vector<double> &,
       const std::vector<double> &);
  virtual ~Hist() = default;

  virtual void fill(ana::observable<ROOT::RVec<Prec>>,
                    ana::observable<ROOT::RVec<Prec>>, double) override;
  virtual std::shared_ptr<TH2> result() const override;
  virtual std::shared_ptr<TH2>
  merge(std::vector<std::shared_ptr<TH2>> const &results) const override;

protected:
  // histogram
  std::shared_ptr<TH2> m_hist; //!
};

template <typename Prec>
class Hist<3, ROOT::RVec<Prec>>
    : public ana::counter::definition<std::shared_ptr<TH3>(
          ROOT::RVec<Prec>, ROOT::RVec<Prec>, ROOT::RVec<Prec>)> {

public:
  Hist(const std::string &, const std::vector<double> &,
       const std::vector<double> &, const std::vector<double> &);
  virtual ~Hist() = default;

  virtual void fill(ana::observable<ROOT::RVec<Prec>>,
                    ana::observable<ROOT::RVec<Prec>>,
                    ana::observable<ROOT::RVec<Prec>>, double) override;
  virtual std::shared_ptr<TH3> result() const override;
  virtual std::shared_ptr<TH3>
  merge(std::vector<std::shared_ptr<TH3>> const &results) const override;

protected:
  // histogram
  std::shared_ptr<TH3> m_hist; //!
};

#include "AnalysisPlugins/HistUtils.h"

template <typename Prec>
Hist<1, Prec>::Hist(const std::string &name, unsigned int nbins, double xmin,
                    double xmax)
    : ana::counter::definition<std::shared_ptr<TH1>(Prec)>() {
  m_hist = HistUtils::makeHist<1, Prec>(nbins, xmin, xmax);
  m_hist->SetName(name.c_str());
}

template <typename Prec>
Hist<1, Prec>::Hist(const std::string &name, const std::vector<double> &xbins)
    : ana::counter::definition<std::shared_ptr<TH1>(Prec)>() {
  m_hist = HistUtils::makeHist<1, Prec>(xbins);
  m_hist->SetName(name.c_str());
}

template <typename Prec>
void Hist<1, Prec>::fill(ana::observable<Prec> x, double w) {
  m_hist->Fill(x.value(), w);
}

template <typename Prec> std::shared_ptr<TH1> Hist<1, Prec>::result() const {
  return m_hist;
}

template <typename Prec>
std::shared_ptr<TH1>
Hist<1, Prec>::merge(std::vector<std::shared_ptr<TH1>> const &results) const {
  auto merged_result =
      std::shared_ptr<TH1>(static_cast<TH1 *>(results[0]->Clone()));
  for (size_t islot = 1; islot < results.size(); ++islot) {
    merged_result->Add(results[islot].get());
  }
  return merged_result;
}

template <typename Prec>
Hist<2, Prec>::Hist(const std::string &name, const std::vector<double> &xbins,
                    const std::vector<double> &ybins)
    : ana::counter::definition<std::shared_ptr<TH2>(Prec, Prec)>() {
  m_hist =
      std::static_pointer_cast<TH2>(HistUtils::makeHist<2, Prec>(xbins, ybins));
  m_hist->SetName(name.c_str());
}

template <typename Prec>
void Hist<2, Prec>::fill(ana::observable<Prec> x, ana::observable<Prec> y,
                         double w) {
  m_hist->Fill(x.value(), y.value(), w);
}

template <typename Prec>
std::shared_ptr<TH2>
Hist<2, Prec>::merge(std::vector<std::shared_ptr<TH2>> const &results) const {
  auto merged_result =
      std::shared_ptr<TH2>(static_cast<TH2 *>(results[0]->Clone()));
  for (size_t islot = 1; islot < results.size(); ++islot) {
    merged_result->Add(results[islot].get());
  }
  return merged_result;
}

template <typename Prec> std::shared_ptr<TH2> Hist<2, Prec>::result() const {
  return m_hist;
}

template <typename Prec>
Hist<3, Prec>::Hist(const std::string &name, const std::vector<double> &xbins,
                    const std::vector<double> &ybins,
                    const std::vector<double> &zbins)
    : ana::counter::definition<std::shared_ptr<TH3>(Prec, Prec, Prec)>() {
  m_hist = std::static_pointer_cast<TH3>(
      HistUtils::makeHist<3, Prec>(xbins, ybins, zbins));
  m_hist->SetName(name.c_str());
}

template <typename Prec>
void Hist<3, Prec>::fill(ana::observable<Prec> x, ana::observable<Prec> y,
                         ana::observable<Prec> z, double w) {
  m_hist->Fill(x.value(), y.value(), z.value(), w);
}

template <typename Prec>
std::shared_ptr<TH3>
Hist<3, Prec>::merge(std::vector<std::shared_ptr<TH3>> const &results) const {
  auto merged_result =
      std::shared_ptr<TH3>(static_cast<TH3 *>(results[0]->Clone()));
  for (size_t islot = 1; islot < results.size(); ++islot) {
    merged_result->Add(results[islot].get());
  }
  return merged_result;
}

template <typename Prec> std::shared_ptr<TH3> Hist<3, Prec>::result() const {
  return m_hist;
}

// vector<T>

template <typename Prec>
Hist<1, ROOT::RVec<Prec>>::Hist(const std::string &name, unsigned int nbins,
                                double xmin, double xmax)
    : ana::counter::definition<std::shared_ptr<TH1>(ROOT::RVec<Prec>)>() {
  m_hist = HistUtils::makeHist<1, Prec>(nbins, xmin, xmax);
  m_hist->SetName(name.c_str());
}

template <typename Prec>
Hist<1, ROOT::RVec<Prec>>::Hist(const std::string &name,
                                const std::vector<double> &xbins)
    : ana::counter::definition<std::shared_ptr<TH1>(ROOT::RVec<Prec>)>() {
  m_hist = HistUtils::makeHist<1, Prec>(xbins);
  m_hist->SetName(name.c_str());
}

template <typename Prec>
void Hist<1, ROOT::RVec<Prec>>::fill(ana::observable<ROOT::RVec<Prec>> xs,
                                     double w) {
  for (size_t ix = 0; ix < xs->size(); ++ix) {
    m_hist->Fill(xs.value()[ix], w);
  }
}

template <typename Prec>
std::shared_ptr<TH1> Hist<1, ROOT::RVec<Prec>>::merge(
    std::vector<std::shared_ptr<TH1>> const &results) const {
  auto merged_result =
      std::shared_ptr<TH1>(static_cast<TH1 *>(results[0]->Clone()));
  for (size_t islot = 1; islot < results.size(); ++islot) {
    merged_result->Add(results[islot].get());
  }
  return merged_result;
}

template <typename Prec>
std::shared_ptr<TH1> Hist<1, ROOT::RVec<Prec>>::result() const {
  return m_hist;
}

template <typename Prec>
Hist<2, ROOT::RVec<Prec>>::Hist(const std::string &name,
                                const std::vector<double> &xbins,
                                const std::vector<double> &ybins)
    : ana::counter::definition<std::shared_ptr<TH2>(ROOT::RVec<Prec>,
                                                    ROOT::RVec<Prec>)>() {
  m_hist =
      std::static_pointer_cast<TH2>(HistUtils::makeHist<2, Prec>(xbins, ybins));
  m_hist->SetName(name.c_str());
}

template <typename Prec>
void Hist<2, ROOT::RVec<Prec>>::fill(ana::observable<ROOT::RVec<Prec>> xs,
                                     ana::observable<ROOT::RVec<Prec>> ys,
                                     double w) {
  if (xs->size() != ys->size()) {
    throw std::runtime_error("x- and y-arrays do not share the same size");
  }
  for (size_t ix = 0; ix < xs->size(); ++ix) {
    m_hist->Fill(xs.value()[ix], ys.value()[ix], w);
  }
}

template <typename Prec>
std::shared_ptr<TH2> Hist<2, ROOT::RVec<Prec>>::merge(
    std::vector<std::shared_ptr<TH2>> const &results) const {
  auto merged_result =
      std::shared_ptr<TH2>(static_cast<TH2 *>(results[0]->Clone()));
  for (size_t islot = 1; islot < results.size(); ++islot) {
    merged_result->Add(results[islot].get());
  }
  return merged_result;
}

template <typename Prec>
std::shared_ptr<TH2> Hist<2, ROOT::RVec<Prec>>::result() const {
  return m_hist;
}

template <typename Prec>
Hist<3, ROOT::RVec<Prec>>::Hist(const std::string &name,
                                const std::vector<double> &xbins,
                                const std::vector<double> &ybins,
                                const std::vector<double> &zbins)
    : ana::counter::definition<std::shared_ptr<TH3>(
          ROOT::RVec<Prec>, ROOT::RVec<Prec>, ROOT::RVec<Prec>)>() {
  m_hist = std::static_pointer_cast<TH3>(
      HistUtils::makeHist<3, Prec>(xbins, ybins, zbins));
  m_hist->SetName(name.c_str());
}

template <typename Prec>
void Hist<3, ROOT::RVec<Prec>>::fill(ana::observable<ROOT::RVec<Prec>> xs,
                                     ana::observable<ROOT::RVec<Prec>> ys,
                                     ana::observable<ROOT::RVec<Prec>> zs,
                                     double w) {
  if (xs->size() != ys->size()) {
    throw std::runtime_error("x- and y-arrays do not share the same size");
  }
  if (xs->size() != zs->size()) {
    throw std::runtime_error("x- and z-arrays do not share the same size");
  }
  for (size_t ix = 0; ix < xs->size(); ++ix) {
    m_hist->Fill(xs.value()[ix], ys.value()[ix], zs.value()[ix], w);
  }
}

template <typename Prec>
std::shared_ptr<TH3> Hist<3, ROOT::RVec<Prec>>::merge(
    std::vector<std::shared_ptr<TH3>> const &results) const {
  auto merged_result =
      std::shared_ptr<TH3>(static_cast<TH3 *>(results[0]->Clone()));
  for (size_t islot = 1; islot < results.size(); ++islot) {
    merged_result->Add(results[islot].get());
  }
  return merged_result;
}

template <typename Prec>
std::shared_ptr<TH3> Hist<3, ROOT::RVec<Prec>>::result() const {
  return m_hist;
}

template class Hist<1, float>;
template class Hist<2, float>;
template class Hist<3, float>;
template class Hist<1, ROOT::RVec<float>>;
template class Hist<2, ROOT::RVec<float>>;
template class Hist<3, ROOT::RVec<float>>;