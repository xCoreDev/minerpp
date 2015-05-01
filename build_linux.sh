
mkdir deps
cd deps
curl -O http://freefr.dl.sourceforge.net/project/boost/boost/1.58.0/boost_1_58_0.tar.gz
tar -zxvf boost_1_58_0.tar.gz
mv boost_1_58_0 boost
cd boost
./bootstrap.sh
cd ../..
cd test
../deps/boost/bjam toolset=gcc cxxflags=-std=gnu++0x release