#oggpnosn 
#hkhr 

#automating testing to compare toolpath generation and machinability approach 

import os 
import subprocess as sp

NoOfTestCases = 2

# fob = open("./testcases/voxel/case_1.txt")
# text = fob.read();


# p = sp.Popen(["time" ,"./sequenceGenerator", "../Case1"], stdin= sp.PIPE, stdout=sp.PIPE)
# print p.communicate(input = text)


#compiling latest version of sequence generator 
p = sp.Popen(["g++", "../src/sequenceGenerator_v1.cpp", "-o", "sequenceGenerator"])
p.communicate()

#for each case create a folder and run the test cases 
for i in range(1, NoOfTestCases+1):
	p = sp.Popen(["mkdir", "Case"+str(i)])
	p.communicate()

	fob = open("./testcases/voxel/case_" + str(i) + ".txt")
	text = fob.read()

	print "-------Case" + str(i) + ":"
	print "Runtime: "
	p = sp.Popen(["time" ,"./sequenceGenerator", "Case" + str(i)], stdin= sp.PIPE, stdout=sp.PIPE)
	p.communicate(input = text)
	print "Machining Time:"
	fob = open("./Case" + str(i) +"/sequence.txt")
	sequence = fob.readline()
	sequence = sequence.split("->")
	sequence = sequence[:-1]

	for orientation in sequence:	
		print orientation, ":",
		fob = open("./Case" + str(i) + "/" + orientation + ".gcode")
		text = fob.read()

		p = sp.Popen(["python" ,"timeEstimator.py"], stdin= sp.PIPE ,stdout= sp.PIPE)
		print p.communicate(input = text)[0]


