#oggpnosn 
#hkhr 

#main script toolpath generator 

import subprocess
from subprocess import CalledProcessError
import sys

SET_OF_ORIENTATIONS = ["xy+", "xy-", "xz+", "xz-", "yz+", "yz-"]

def parseHeightmapAndModel(dump):
	state = {"heightmaps": [], "model": "", "sequence":[], "scale":1}
	dump = dump.split("\n")
	
	state["scale"] = int(dump[0])
	indexes = {}
	for i in range(len(SET_OF_ORIENTATIONS)):
		index = dump.find(SET_OF_ORIENTATIONS[i])
		if index != -1:
			indexes[SET_OF_ORIENTATIONS[i]] = index
	pointOfInterest = sorted(indexes, key=lambda x: indexes[x])

	for index in pointOfInterest:
		orientation = dump[index:index+3]




stlFilename = sys.argv[1]

try:
	command = ["./heightmapGenerator", stlFilename]
	generatorOutput = subprocess.check_output(command)
	print generatorOutput
except(CalledProcessError):
	print "Error: Something went terribly wrong! No i am not ex-microsoft employee"






