mkdir out
cd out
cmake ..
msbuild glGame.slnx /p:Configuration=Release /p:Platform=x64

cd Release/bin
move Release\OUTPUT_BUILD.exe .
move Release\OUTPUT_BUILD.pdb .
move Release\unit.exe .
move Release\unit.pdb .
