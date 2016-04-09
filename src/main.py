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
import numpy as np 
from koko.fab.image import Image

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

def  generateGcodeFromPath(paths, feed, jog, plunge):
	''' Convert the path from the previous panel into a g-code string 
	'''
	
	# Check to see if all of the z values are the same.  If so,
	# we can use 2D cutting commands; if not, we'll need
	# to do full three-axis motion control
	zmin = paths[0].points[0][2]
	flat = True
	for p in paths:
		if not all(pt[2] == zmin for pt in p.points):
			flat = False

	gcode = ""

	gcode += "G17\n"	# XY plane
	gcode += "G21\n"	# Inch mode
	gcode += "G40\n"	# Cancel tool diameter compensation
	gcode += "G49\n"	# Cancel tool length offset
	gcode += "G54\n"	# Coordinate system 1
	gcode += "G80\n"	# Cancel motion
	gcode += "G90\n"	# Absolute programming
	gcode += "G94\n"	# Feedrate is per minute

	scale = 1 # mm units


	gcode += "F%0.4f\n" % (60*scale*feed)  # Feed rate


	# Move up before starting spindle
	gcode += "G00Z%0.4f\n" % (scale*jog)
	
	xy  = lambda x,y:   (scale*x, scale*y)
	xyz = lambda x,y,z: (scale*x, scale*y, scale*z)


	for p in paths:

		# Move to the start of this path at the jog height
		gcode += "G00X%0.4fY%0.4fZ%0.4f\n" %(xyz(p.points[0][0], p.points[0][1], jog))

		# Plunge to the desired depth
		gcode += "G01Z%0.4f F%0.4f\n" % (p.points[0][2]*scale, 60*scale*plunge)

		# Restore XY feed rate
		gcode += "F%0.4f\n" % (60*scale*feed)

		# Cut each point in the segment
		for pt in p.points:
			if flat:	gcode += "X%0.4fY%0.4f\n" % xy(*pt[0:2])
			else:	   gcode += "X%0.4fY%0.4fZ%0.4f\n" % xyz(*pt)

		# Lift the bit up to the jog height at the end of the segment
		gcode += "Z%0.4f\n" % (scale*jog)


	gcode += "M30\n" # program end and reset
	return gcode 

def run_rough(img, values):
	""" @brief Calculates a rough cut toolpath
		@param img Input image
		@returns Dictionary with 'paths' defined
	"""
	global state 
	# Save image's original z values (which may have been None)
	old_zvals = img.zmin, img.zmax

	if img.zmin is not None and img.zmax is not None and img.dz:
		values['top']	= img.zmax/state["scale"] #to account for scaling 
		values['bottom'] = img.zmin
	

	# We only need an overlap value if we're cutting more than one offsets
	if values['offsets'] != 1:
		v = values['overlap']
		
	else:
		values['overlap'] = 1

	# Figure out the set of z values at which to cut
	heights = [values['top']]
	while heights[-1] > values['bottom']:
		heights.append(heights[-1]-values['step'])
	heights[-1] = values['bottom']

	# Loop over z values, accumulating samples
	i = 0
	np.set_printoptions(threshold='nan')
	
	# fob =open("./transforms/img", "w")
	# fob.write(img.array.__str__())
	# fob.close()
	
	self_paths = []
	last_image = None
	last_paths = []
	for z in heights:
		i += 1

		L = img.threshold(z)
		#print L.array; print "---"
		L.array *= 100

		if last_image is not None and L == last_image:
			paths = [p.copy() for p in last_paths]
		else:
			distance = L.distance()

			# fob =open("./transforms/tf_"+str(z), "w")
			# fob.write(distance.array.__str__())
			# fob.close()

			paths = distance.contour(
				values['diameter'], values['offsets'], values['overlap']
			)

		for p in paths:	p.set_z(z-values['top'])

		last_paths = paths
		last_image = L
		self_paths += paths

	

	# Restore z values on image
	img.zmin, img.zmax = old_zvals

	return self_paths




def getHeightmap(arg):
	global state

	split_arg = arg.split("\n")
	dimensions_arg = split_arg[0].split(" ");
	heightmap_arg = split_arg[1].split(" ")
	
	scale = state["scale"]
	length = int(dimensions_arg[0])
	width = int(dimensions_arg[1])
	array = [[[int(heightmap_arg[i*width+j])] for j in range(width)] for i in range(length)]
	
	#normalization of 16 bit array
	array = np.array(array);
	zmax = max(array.flatten()) - 1; 
	array = array*65535/(zmax * scale) #to account for scale
	return (length, width, zmax, scale, array)



def generateGcode(arg):
	global diameter, step, feed
	command = ["./operationPlaner"]
	p = Popen(command, stdout=PIPE, stdin=PIPE, stderr=STDOUT)	
	generatorOutput = p.communicate(input=arg)[0]
	plan = json.loads(generatorOutput)
	
	length, width, zmax, scale, array = getHeightmap(arg)
	values = {
		'diameter':	diameter,
		'offsets':	 -1,
		'overlap':	 0.5,
		'step':		step,
		'feed': feed,
		 'plunge': 2.5,
		 'jog': 2,
		 'zmax' : zmax,
		 'zmin' : 0
		}
	
	img = Image(length, width, scale,  zmax=zmax, array=array, channels=1, depth = 16)
	#getting paths 
	paths = run_rough(img, values)
	#generating gcode from paths
	gcode = generateGcodeFromPath(paths, values["feed"], values["jog"], values["plunge"])
	return gcode







stlFilename = sys.argv[1]
diameter = float(sys.argv[2])
step = float(sys.argv[3])
feed = float(sys.argv[4])

try:
	command = ["./heightmapGenerator", stlFilename]
	generatorOutput = subprocess.check_output(command)
	state = json.loads(generatorOutput)

	argumentsToOperationPlanner = parseState(state)
	
	pool = Pool(len(state["heightmaps"]))
	gcodes = pool.map(generateGcode, tuple(argumentsToOperationPlanner)) 
	
	# gcodes = generateGcode(argumentsToOperationPlanner[0])
	
	print gcodes
except(CalledProcessError):
	print "Error: Something went terribly wrong! No i am not ex-microsoft employee"





