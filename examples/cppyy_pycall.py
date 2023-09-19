import cppyy
import ROOT
from ROOT import ana

df = ana.dataflow['Tree'](['hww.root'],'mini')

runNumber = df.read['int']('runNumber')

cppyy.cppdef('''auto plusOne = std::function([](int x){return x+1;});''')
print(cppyy.gbl.plusOne)

plusOne = df.define(cppyy.gbl.plusOne)
print(plusOne)

runNumberPlusOne = plusOne.evaluate(runNumber)
print(runNumberPlusOne)