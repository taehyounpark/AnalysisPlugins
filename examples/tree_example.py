import ROOT
from ROOT import ana

df = ana.dataflow['Tree'](['hww.root'],'mini')
print(df)

mc_weight = df.read['float']('mcWeight')
print(mc_weight)

test_fn = ROOT.gInterpreter.Declare('''auto test_fn = std::function([](float x){return x;});''')
print(ROOT.test_fn)
test = df.define['std::function<float(float)>'](ROOT.test_fn)
print(test)

import cppyy
cppyy.load_library('/home/thpark/ana/analogical-benchmarks/build/AnalysisPlugins/libAnalysisPlugins.so')