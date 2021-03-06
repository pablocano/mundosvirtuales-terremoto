mkdir build32 & pushd build32
cmake -G "Visual Studio 14 2015" ..
popd
mkdir build64 & pushd build64
cmake -G "Visual Studio 14 2015 Win64" ..
popd
mkdir bin
mkdir lib
cmake --build build32 --config Release
cmake --build build64 --config Release
cmake --build build32 --config Debug
cmake --build build64 --config Debug