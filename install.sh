#!/bin/bash

echo "Updating Repositories..."
apt-get update

echo "Installing..."
apt-get install git g++ python

echo "Removing Previous Boost Installation..."
rm /usr/local/lib/libboost*
rm -r /usr/local/include/boost

cd ..
echo "Downloading Boost..."
wget -c 'http://sourceforge.net/projects/boost/files/boost/1.60.0/boost_1_60_0.tar.bz2/download'
echo "Extracting and Installing Boost..."
tar xf download
cd boost_1_60_0
./bootstrap.sh
./b2 install

cd ../ToolpathGenerator/src
echo "Compiling Binaries..."
g++ ./heightmapGenerator.cpp -o heightmapGenerator -std=c++11
g++ ./operationPlanner.cpp -o operationPlaner -std=c++11

cd ..

echo "Done."
