#oggpnosn 
#hkhr 

#main script toolpath generator 

import subprocess
from subprocess import CalledProcessError
import sys



stlFilename = sys.argv[1]

try:
	command = ["./heightmapGenerator", stlFilename]
	generatorOutput = subprocess.check_output(command)
	print generatorOutput
except(CalledProcessError):
	print "Error: Something went terribly wrong! No i am not ex-microsoft employee"






