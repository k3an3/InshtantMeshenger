# Building from source 

## Dependencies
Debian/Ubuntu:

```apt-get install boost-system1.55-dev libcppnetlib-dev libboost-thread-dev cmake build-essential git libcrypto++-dev``

If building the Qt GUI ```apt-get install qt5-default```

## Install

cd G32_InshtantMeshenger/libmeshenger
mkdir build && cd build
cmake ..
make