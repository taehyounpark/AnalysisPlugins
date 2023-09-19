# AnalysisPlugins

## Features

The following plugins are implemented for ROOT/xAOD analysis:

### Input datasets

- `TTree`: use `ana::dataflow<Tree>`.
- `xAOD`: use `ana::dataflow<Event>`

### Aggregations

- `Hist<NDim,Prec>` provides histogram support for `TH1`-based classes. 
- `Scan<NDim,Prec>` fills an n-dimensional array at each point of a fixed grid.
- `MiniTree<Columns...>`: outputs a `TTree` with specified column data types.

## How to build
```sh
$ # setup AnalysisBaes environment & project
$ setupATLAS
$ mkdir -p AnalysisFramework/ && cd AnalysisFramework/
$ asetup AnalysisBase,22.2.110,here

$ # clone repositories
$ git clone https://github.com/taehyounpark/analogical.git
$ git clone https://github.com/taehyounpark/AnalysisPlugins.git

$ # delete CMakeLists.txt problematic for AnalysisBase environment
$ rm analogical/CMakeLists.txt analogical/tests/CMakeLists.txt

$ # configure build & compile
$ cd ../ && mkdir -p build/ && cd build/
$ cmake ../AnalysisFramework/
$ make -j4
```