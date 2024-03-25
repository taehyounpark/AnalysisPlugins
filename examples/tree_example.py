import ROOT
from ROOT import queryosity as qty

df = qty.dataflow()

ds = df.load( qty.dataset.input['Tree'](['hww.root'], 'mini') )

print(df,ds)

mc_weight = ds.read( qty.dataset.column['float']('mcWeight') )
print(mc_weight)

test_fn = ROOT.gInterpreter.Declare('''auto test_fn = std::function([](float x){return x;});''')
print(ROOT.test_fn)
test = df.define['std::function<float(float)>'](ROOT.test_fn)
print(test)

import cppyy
cppyy.load_library('/home/thpark/ana/queryosity-benchmarks/build/qhep/libqhep.so')