#oggpnosn 
#hkhr 

#main script toolpath generator 

import subprocess
from subprocess import CalledProcessError
import sys
from multiprocessing import Pool 
import json
from random import randrange
from subprocess import Popen, PIPE, STDOUT

SET_OF_ORIENTATIONS = ["xy+", "xy-", "xz+", "xz-", "yz+", "yz-"]


def parseState(state):
	args = []
	for heightmap in state["heightmaps"]:
		arg = str(heightmap["length"]) + " " +str(heightmap["width"])  + " "
		if heightmap["orientation"] == "xy+" or heightmap["orientation"] == "xy-":
			arg += str(state["model"]["height"]) + "\n"
		elif heightmap["orientation"] == "xz+" or heightmap["orientation"] == "xz-":
			arg += str(state["model"]["width"]) + "\n"
		elif heightmap["orientation"] == "yz+" or heightmap["orientation"] == "yz-":
			arg += str(state["model"]["length"]) + "\n"
		arg += heightmap["raw"]
		args.append(arg)
	return args 


def generateGcode(arg):
	command = ["./operationPlaner"]
	p = Popen(command, stdout=PIPE, stdin=PIPE, stderr=STDOUT)    
	generatorOutput = p.communicate(input=arg)[0]
	
	plan = json.loads(generatorOutput)
	return plan



stlFilename = sys.argv[1]

try:
	command = ["./heightmapGenerator", stlFilename]
	generatorOutput = subprocess.check_output(command)
	state = json.loads(generatorOutput)
	argumentsToOperationPlanner = parseState(state)
	pool = Pool(len(state["heightmaps"]))
	gcodes = pool.map(generateGcode, tuple(argumentsToOperationPlanner)) 
	print json.dumps(gcodes)
except(CalledProcessError):
	print "Error: Something went terribly wrong! No i am not ex-microsoft employee"






