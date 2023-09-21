#pragma once

#include <string>
#include <vector>
#include <utility>
#include <memory>

#include <ROOT/RVec.hxx>
#include <ROOT/RDataSource.hxx>

#include "ana/analogical.h"

class RDS : public ana::dataset::input<RDS>
{

private:
  using RDataSource   = ROOT::RDF::RDataSource;

public:
	class Reader;

	template <typename T>
	class Column;

public:
	RDS(std::unique_ptr<RDataSource> rds);
	~RDS() = default;

	ana::dataset::partition allocate();
	std::unique_ptr<Reader> read(const ana::dataset::range& part) const;

	void start();
	void finish();

protected:
  std::unique_ptr<RDataSource> m_rds;

};


class RDS::Reader : public ana::dataset::reader<Reader>
{

public:
	Reader(RDataSource& rds);
	~Reader() = default;

	template <typename T>
	std::unique_ptr<Column<T>> read(const ana::dataset::range&, const std::string&) const;

 	void start(const ana::dataset::range& part);
	void next(const ana::dataset::range& part, unsigned long long entry);
 	void finish(const ana::dataset::range& part);

protected:
  RDataSource* m_rds;
  long long    m_current;

};

template <typename T>
class RDS::Column : public ana::column::reader<T>
{

public:
	Column(T** cursor) :
		m_cursor(cursor)
	{}
	~Column() = default;

	virtual const T& read(const ana::dataset::range&, unsigned long long) const override
	{
		return static_cast<const T&>(**m_cursor);
	}

protected:
	T** m_cursor;

};

template <typename T>
std::unique_ptr<RDS::Column<T>> RDS::Reader::read(const ana::dataset::range& part, const std::string& name) const
{
  auto columnReaders = m_rds->GetColumnReaders<T>(name.c_str());
	return std::make_unique<Column<T>>(columnReaders[part.slot]);
}
