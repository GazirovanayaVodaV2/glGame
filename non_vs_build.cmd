mkdir out
cd out
cmake ..
msbuild glGame.slnx

cd Debug/bin
move Debug\OUTPUT_BUILD.exe .
move Debug\OUTPUT_BUILD.pdb .
move Debug\unit.exe .
move Debug\unit.pdb .
