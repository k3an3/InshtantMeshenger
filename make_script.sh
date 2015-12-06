#/bin/bash
cd libmeshenger/build
make
cd ..
cd bin_crypto
./keygen me
cp me.priv me.pub ../bin_gui
cd ..
