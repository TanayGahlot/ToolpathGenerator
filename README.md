# ToolpathGenerator(Beta)
Automating Toolpath Generation of 3D objects for 3-Axis CNC 

ToolpathGenerator automates toolpath planing process involved in toolpath generation of 3D objects for 3-Axis CNC. This work seeks to reduce the barrier faced by the entry level makers and designers involved in CNC machining. 

#Usage
##Dependancies
The voxelizer built into the system requires [boost library for c+](www.boost.org) to compile. Please note that, the [cvmlcpp library](http://tech.unige.ch/cvmlcpp/) and [omptl library](http://tech.unige.ch/omptl/) has been modified and integrated into the source code.

The voxelizer currently is known not to work on Mac, windows remains to be tested.

##Compiling
###Stand-alone Voxelizer
If you wish to only voxelize the stl file without generating toolpaths, you need to compile as:
```
>g++ -std=c++11 ./src/standalone_voxelizer.cpp -o standalone_voxelizer
```
###ToolpathGenerator without Voxelizer
To run ToolpathGenerator without voxelizing, compile:
```
>g++ -std=c++11 ./src/run_without_voxelizer.cpp -o run_without_voxelizer
```
###ToolpathGenerator with Voxelizer
To run ToolpathGenerator with voxelizer, compile:
```
>g++ -std=c++11 ./src/run_with_voxelizer.cpp -o run_with_voxelizer
```

##Executing
###Stand-alone Voxelizer
Standalone voxelizer gives choice of input and output through parameters:

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
