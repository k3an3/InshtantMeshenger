#/bin/bash
rm -rf libmeshenger/build
mkdir libmeshenger/build
cd libmeshenger/build
cmake ..
make
cd ..
cd bin_crypto
./keygen me
cp me.priv me.pub ../bin_gui
cd ..
