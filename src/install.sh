#!/bin/bash

apt-get update

apt-get install git g++

wget -c 'http://sourceforge.net/projects/boost/files/boost/1.50.0/boost_1_50_0.tar.bz2/download'
tar xf download
cd boost_1_50_0
./bootstrap.sh
./b2 install

g++ ./heightmapGenerator.cpp -o heightmapGenerator -std=c++11
g++ ./operationPlanner.cpp -o operationPlaner -std=c++11

sudo npm install
