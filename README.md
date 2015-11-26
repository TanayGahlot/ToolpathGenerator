# ToolpathGenerator(Beta)
Automating Toolpath Generation for 3-Axis CNC 

ToolpathGenerator automates toolpath planing process involved in toolpath generation for 3-Axis CNC. This work seeks to reduce barrier faced by entry level makers and designer involved in CNC machining. 

#Usage
##Voxelizer
Voxelizer requires [cvmlcpp library](http://tech.unige.ch/cvmlcpp/) to run.
###Compiling
```
>g++ -std=c++11 ./src/voxelizer.cpp -o voxelizer
```
###Running it on stl file
```
>./voxelizer case_1.stl > case_1.txt
```
This generates voxelized model in output file.
##Sequence Generator
###Compiling 
```
>g++ -std=c++11 ./src/sequenceGenerator.cpp -o sequenceGenerator
```
###Running it on test case
```
>mkdir Case1
>./sequenceGenerator Case1 <test/testcases/voxel/case_1.txt 
```
Results will be stored _Case1_ folder
