mkdir build
cd build
cmake .. -G "Visual Studio 16 2019"
cmake --build . --target ALL_BUILD --config Debug 
cd ..
mkdir dll
cd build/Debug
move *.dll ../../dll
cd ../.. 

cd algorithms
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019"
cmake --build . --target ALL_BUILD --config Debug 
cd Debug
move *.dll ../../../dll
cd ../../.. 