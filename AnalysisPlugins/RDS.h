#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <ROOT/RDataSource.hxx>
#include <ROOT/RVec.hxx>

#include "ana/analogical.h"

class RDS : public ana::dataset::input<RDS> {

private:
  using RDataSource = ROOT::RDF::RDataSource;

public:
  class Reader;

  template <typename T> class Column;

public:
  RDS(std::unique_ptr<RDataSource> rds);
  ~RDS() = default;

  ana::dataset::partition allocate();
  std::unique_ptr<Reader> open(const ana::dataset::range &part) const;

  template <typename T>
  std::unique_ptr<Column<T>> read(const ana::dataset::range &,
                                  const std::string &) const;

  void initialize();
  void finalize();

protected:
  std::unique_ptr<RDataSource> m_rds;
};

class RDS::Reader : public ana::dataset::row {

public:
  Reader(RDataSource &rds);
  ~Reader() = default;

  virtual void initialize(const ana::dataset::range &part) override;
  virtual void execute(const ana::dataset::range &part,
                       unsigned long long entry) override;
  virtual void finalize(const ana::dataset::range &part) override;

protected:
  RDataSource *m_rds;
  long long m_current;
};

template <typename T> class RDS::Column : public ana::dataset::column<T> {

public:
  Column(T **cursor) : m_cursor(cursor) {}
  ~Column() = default;

  virtual const T &read(const ana::dataset::range &,
                        unsigned long long) const override {
    return static_cast<const T &>(**m_cursor);
  }

protected:
  T **m_cursor;
};

template <typename T>
std::unique_ptr<RDS::Column<T>> RDS::read(const ana::dataset::range &part,
                                          const std::string &name) const {
  auto columnReaders = m_rds->GetColumnReaders<T>(name.c_str());
  return std::make_unique<Column<T>>(columnReaders[part.slot]);
}
