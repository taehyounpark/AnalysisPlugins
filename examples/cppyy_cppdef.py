import cppyy
import ROOT
from ROOT import ana

df = cppyy.gbl.ana.dataflow['Tree']([],'')

cppyy.cppdef('''
  auto df = ana::dataflow<Tree>({"hww.root"},"mini");
''')

cppyy.cppdef('''
  auto runNumber = df.read<int>("runNumber");
  auto plusOne = df.define([](int x){return x+1;});
  auto runNumberPlusOne = plusOne(runNumber);
''')

print(cppyy.gbl.runNumber)
print(cppyy.gbl.plusOne)
print(cppyy.gbl.runNumberPlusOne)