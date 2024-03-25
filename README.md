# qhep

## Features

The following plugins are implemented for ROOT/xAOD analysis:

### Input datasets

- `Tree`: support for `TTree`.
- `Event`: support for ATLAS xAOD format.

### Aggregations

- `Hist<NDim,Prec>`: fill an N-dimensional histogram of specified precision as provided by `TH1`-derived classes. 
- `Tree::Snapshot<Columns...>`: output a snapshot of entries as another `TTree` snapshot with specified columns.

## How to build
```sh
# setup AnalysisBaes environment & project
setupATLAS
mkdir -p AnalysisFramework/ && cd AnalysisFramework/
asetup AnalysisBase,22.2.110,here

# clone repositories
git clone https://github.com/taehyounpark/queryosity.git
git clone https://github.com/taehyounpark/qhep.git

# delete CMakeLists.txt problematic for AnalysisBase environment
rm queryosity/tests/CMakeLists.txt

# configure build & compile
cd ../ && mkdir -p build/ && cd build/
cmake ../AnalysisFramework/
make -j4
```