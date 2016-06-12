#oggpnosn
#hkhr

#time estimator: estimates time it take to machine the input gcode

import sys
import math

#default feedrate
feedrate = 250

#assigning current x,y,z position to origin
xcurrent = 0
ycurrent = 0
zcurrent = 0

#assigning machining time ton be zero
machineTime = 0

#taking gcode input from stdin
filepath = sys.argv[1]
fob = open(filepath)
text = fob.read(); fob.close()
for command in text.split("\n"):
	command = command.split(' ')
	if command[0] == "G00" or command[0] == "G01":
		xnext = xcurrent
		ynext = ycurrent
		znext = zcurrent
		for spec in command[1:]:
			if spec:
				if spec[0] == "X":
					xnext = float(spec[1:])
				elif spec[0] == "Y":
					ynext = float(spec[1:])
				elif spec[0] == "Z":
					znext = float(spec[1:])
		distance = math.sqrt((xnext - xcurrent)**2 + (ynext - ycurrent)**2 + (znext - zcurrent)**2)
		machineTime += distance*1.0/feedrate
		xcurrent = xnext
		ycurrent = ynext
		zcurrent = znext

print machineTime
