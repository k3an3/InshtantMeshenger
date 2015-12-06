#/bin/bash
cd libmeshenger/build
make
cd ..
cd bin_crypto
./keygen me
cp me.priv me.pub ../bin_gui
cd ..

##wget files.drahos.me/jake.pub
##./chat_test joe.priv keane mesh1 mesh2 mesh3 jake

