#include "AnalysisPlugins/Tree.h"

#include "TROOT.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"

Tree::Tree(const std::vector<std::string>& allFiles, const std::string& treeName) :
	m_allFiles(allFiles),
	m_treeName(treeName)
{}

ana::dataset::partition Tree::allocate()
{
	ROOT::EnableThreadSafety();
	ROOT::EnableImplicitMT();

	TDirectory::TContext c;
	ana::dataset::partition parts;

	// offset to account for global entry position
	long long offset = 0ll;
	size_t islot=0;
	for (const auto& filePath : m_allFiles) {
		// check file
		std::unique_ptr<TFile> file(TFile::Open(filePath.c_str()));
		if (!file) {
			continue;
		} else if (file->IsZombie()) {
			continue;
		}

		// check tree
		auto tree = file->Get<TTree>(m_treeName.c_str());
		if (!tree) {
			continue;
		}

		// tree good
		m_goodFiles.push_back(filePath);

		// add tree clusters
		auto fileEntries = tree->GetEntries();
		auto clusterIterator = tree->GetClusterIterator(0);
		long long start = 0ll, end = 0ll;
		while ( (start=clusterIterator.Next())<fileEntries ) {
			end = clusterIterator.GetNextEntry();
			parts.add_part(islot++, offset+start, offset+end);
		}
		// remember offset for next file
		offset += fileEntries;
	}

	return parts;
}

std::unique_ptr<Tree::Reader> Tree::read(const ana::dataset::range&) const
{
	auto tree = std::make_unique<TChain>(m_treeName.c_str(),m_treeName.c_str());
	tree->ResetBit(kMustCleanup);
	for (auto const& filePath : m_goodFiles) {
		tree->Add(filePath.c_str());
	}
	return std::make_unique<Reader>(std::move(tree));
}

Tree::Reader::Reader(std::unique_ptr<TTree> tree) :
	m_tree(std::move(tree))
{
	m_treeReader = std::make_unique<TTreeReader>(m_tree.get());
}

void Tree::Reader::start(const ana::dataset::range& part)
{
	m_treeReader->SetEntriesRange(part.begin,part.end);
}

void Tree::Reader::next(const ana::dataset::range&, unsigned long long entry)
{
	m_treeReader->SetEntry(entry);
}

void Tree::Reader::finish(const ana::dataset::range&)
{
	// nothing to do
}
