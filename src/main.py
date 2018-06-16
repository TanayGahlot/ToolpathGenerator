"""main script that wraps toolpath generator"""
#oggpnosn
#hkhr
import subprocess
import json
from random import randrange
import argparse
from subprocess import Popen, PIPE, STDOUT, CalledProcessError
import numpy as np
from koko.fab.image import Image
import time

#state variables
#possible set of orientation along with their maping used in model
SET_OF_ORIENTATIONS = ["xy+", "xy-", "xz+", "xz-", "yz+", "yz-"]
ORIENTATION_CODE = {"xy+":11, "xy-":12, "yz+":13, "yz-":14, "xz+":15, "xz-":16}


def parse_state(state):
    """"
        prepare input for gcodeGenerator
        i/p:state varible
        o/p:list of tuple <formatted argument, orientation>
    """
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
        args.append((arg, heightmap["orientation"]))
    return args

def generate_gcode_from_path(paths, feed, jog, plunge):
    '''
        Convert the path from the previous panel into a g-code string
    '''
    gcode = ""
    gcode += "G17\n"    # XY plane
    gcode += "G21\n"    # Inch mode
    gcode += "G40\n"    # Cancel tool diameter compensation
    gcode += "G49\n"    # Cancel tool length offset
    gcode += "G54\n"    # Coordinate system 1
    gcode += "G80\n"    # Cancel motion
    gcode += "G90\n"    # Absolute programming
    gcode += "G94\n"    # Feedrate is per minute

    # mm units, note this scale is different from global scale thats defined,
    # this is used primarily for units conversion
    scale = 1
    gcode += "F%0.4f\n" % (60*scale*feed)  # Feed rate

    # Move up before starting spindle
    gcode += "G00 Z%0.4f\n" % (scale*jog)

    xyz = lambda x, y, z: (scale*x, scale*y, scale*z)
    for p in paths:
        # Move to the start of this path at the jog height
        gcode += "G00 X%0.4f Y%0.4f Z%0.4f\n" %(xyz(p.points[0][0], p.points[0][1], jog))
        # Plunge to the desired depth
        gcode += "G01 Z%0.4f F%0.4f\n" % (p.points[0][2]*scale, 60*scale*plunge)
        # Restore XY feed rate
        gcode += "F%0.4f\n" % (60*scale*feed)
        # Cut each point in the segment
        for pt in p.points:
            gcode += "G01 X%0.4f Y%0.4f Z%0.4f\n" % xyz(*pt)
        # Lift the bit up to the jog height at the end of the segment
        gcode += "G01 Z%0.4f\n" % (scale*jog)
    gcode += "M30\n" # program end and reset
    return gcode

def get_regionmap(plan):
    """
        converts raw regionmap string to two-dimnesional regionmap
    """
    length = plan["regionmap"]["length"]
    width = plan["regionmap"]["width"]
    raw = plan["regionmap"]["raw"].split(" ")
    regionmap = [[int(raw[i*width +j]) for j in range(width)]for i in range(length)]
    return regionmap

def to_be_machined(orientation, x, y, z):
    """
        checks if given orientation is suppose to machine the point (x, y, z)
    """
    global SCALE, MODEL, MODEL_LENGTH, MODEL_WIDTH, MODEL_HEIGHT
    z *= SCALE
    z = int(z) #to have integeral index
    if orientation == "xy+":
        return MODEL[x][y][z-1] == ORIENTATION_CODE[orientation]
    elif orientation == "xy-":
        return MODEL[x][y][MODEL_HEIGHT - z -1] == ORIENTATION_CODE[orientation]
    elif orientation == "yz+":
        return MODEL[z-1][x][y] == ORIENTATION_CODE[orientation]
    elif orientation == "yz-":
        return MODEL[MODEL_LENGTH-z-1][x][y] == ORIENTATION_CODE[orientation]
    elif orientation == "xz+":
        return MODEL[x][z-1][y] == ORIENTATION_CODE[orientation]
    elif orientation == "xz-":
        return MODEL[x][MODEL_WIDTH-z -1][y] == ORIENTATION_CODE[orientation]

def generate_bitmap(operation, orientation, regionmap, z):
    """
        generate bitmap for an operation's z-height level
    """
    global SCALE
    length = len(regionmap)
    width = len(regionmap[0])
    bitmap = [[False for x in range(width)]for y in range(length)]
    operationList = [int(region) for region in operation["regionlist"].split(" ") if region != ""]


    for x in range(length):
        for y in range(width):
            if regionmap[x][y] in operationList:
                if to_be_machined(orientation, x, y, z):
                    bitmap[x][y] = [0]     #shoundnt be cut
                else:
                    bitmap[x][y] = [1] #should be cut
            else:
                bitmap[x][y] = [1]
            # print int(bitmap[x][y][0]),
        # print "-"
    # print "---------------------"
    img = Image(length, width, SCALE, channels=1, depth=8)
    img.array = np.array(bitmap, dtype=np.uint8)

    return img

def get_max_height(orientation):
    """
        get height of bounding box in given orientation
    """
    global MODEL_LENGTH, MODEL_WIDTH, MODEL_HEIGHT
    if orientation == "xy+" or orientation == "xy-":
        return MODEL_HEIGHT
    elif orientation == "xz+" or orientation == "xz-":
        return MODEL_WIDTH
    elif orientation == "yz+" or orientation == "yz-":
        return MODEL_LENGTH

def run_rough(img, values, plan, orientation):
    """ @brief Calculates a rough cut toolpath
        @param img Input image
        @returns Dictionary with 'paths' defined
    """
    global STATE

    regionmap = get_regionmap(plan)
    operations = plan["operationPlan"]
    # Save image's original z values (which may have been None)
    old_zvals = img.zmin, img.zmax

    global_paths = []
    for operation in operations:
        # Figure out the set of z values at which to cut
        values["top"] = operation["startHeight"]/SCALE
        values["bottom"] = (operation["startHeight"] - operation["depth"] + 1)/SCALE
        heights = [values['top']]
        while heights[-1] > values['bottom']:
            heights.append(heights[-1]-values['step'])
        heights[-1] = values['bottom']


        # Loop over z values, accumulating samples

        np.set_printoptions(threshold='nan')

        self_paths = []
        last_image = None
        last_paths = []
        for z in heights:

            L = generate_bitmap(operation, orientation, regionmap, z)
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

            for p in paths: p.set_z(z-(get_max_height(orientation))/SCALE)

            last_paths = paths
            last_image = L
            self_paths += paths

        global_paths += self_paths


    # Restore z values on image
    img.zmin, img.zmax = old_zvals

    return global_paths

def get_heightmap(arg):
    """
        parse heightmap from given input
    """
    global STATE

    split_arg = arg.split("\n")
    dimensions_arg = split_arg[0].split(" ")
    heightmap_arg = split_arg[1].split(" ")

    scale = STATE["scale"]
    length = int(dimensions_arg[0])
    width = int(dimensions_arg[1])
    array = [[[int(heightmap_arg[i*width+j])] for j in range(width)] for i in range(length)]

    #normalization of 16 bit array
    array = np.array(array)
    zmax = max(array.flatten()) - 1
    array = array*65535/((zmax) * scale) #to account for scale
    return (length, width, zmax, scale, array)

def generate_gcode(*args):
    """
        generate gcode for heightmap in a given orientation
        note: [toBeImproved]heightmap passed as formatted string isnt a
        good way to structure the code
    """
    arg = args[0][0]
    orientation = args[0][1]
    global diameter, step, feed
    command = ["./scripts/operationPlanner"]
    p = Popen(command, stdout=PIPE, stdin=PIPE, stderr=STDOUT)
    generatorOutput = p.communicate(input=arg)[0]
    plan = json.loads(generatorOutput)

    length, width, zmax, scale, array = get_heightmap(arg)
    values = {
        'diameter':    diameter,
        'offsets':     -1,
        'overlap':     0.5,
        'step':        step,
        'feed': feed,
        'plunge': 2.5,
        'jog': 2,
        'zmax' : zmax,
        'zmin' : 0
        }

    img = Image(length, width, scale, zmax=zmax, array=array,
                provides_array=True, channels=1, depth=16)
    #getting paths
    paths = run_rough(img, values, plan, orientation)
    #generating gcode from paths
    gcode = generate_gcode_from_path(paths, values["feed"], values["jog"], values["plunge"])
    return gcode

def get_model(state):
    """
        parse raw model into three dimesional array(list)
    """
    length = state["model"]["length"]
    width = state["model"]["width"]
    height = state["model"]["height"]
    raw = state["model"]["raw"].split(" ")
    model = [[[int(raw[(z + (x*(width*height)) + (y*height))])
               for z in range(height)]for y in range(width)]
             for x in range(length)]
    return model

def get_commandline_parser():
    """it parses commandline arguments."""
    parser = argparse.ArgumentParser(description='Toolpath generator.')
    parser.add_argument('--stl-filepath', help='filpath of stl file.')
    parser.add_argument('--diameter', help='Diameter of toolbit.')
    parser.add_argument('--step-size', help='Step size of the CNC machine.')
    parser.add_argument('--feed-rate', help='Feed rate of CNC machine.')
    parser.add_argument('--calculate-time', help='Flag to print time.',
                        type=bool)
    return parser

def generate_instructions(diameter, step, feed, state):
    """it generates the instruction and gcode for the stl_file."""
    global MODEL, MODEL_LENGTH, MODEL_WIDTH, MODEL_HEIGHT, SCALE
    #formatting input for gcode generator
    #input: <heightmap, orientation>
    argumentsToOperationPlanner = parse_state(state)
    response = str(MODEL_LENGTH/SCALE) + "," + str(MODEL_WIDTH/SCALE) + "," +\
               str(MODEL_HEIGHT/SCALE) + " "
    sequence = []
    fileNames = []
    sequence_time = []
    for i in range(len(state["heightmaps"])):
        start_time = time.time()
        orientation = state["heightmaps"][i]["orientation"]
        # response.append({"orientation": orientation, "gcode": gcodes[i]})

        gcode = generate_gcode(argumentsToOperationPlanner[i])

        randomFileName = "k_" + str(i+1) + "_"  +\
                         str(randrange(123422, 1212121212)) + ".nc"
        fileNames.append(randomFileName)

        fob = open("./public/download/" + randomFileName, "w")
        fob.write(gcode)
        fob.close()
        end_time = time.time()
        sequence_time.append(end_time-start_time)
        sequence.append(orientation)

    response += ",".join(sequence) + " "
    response += ",".join(fileNames) + " "

    return (sequence_time, response)


if __name__ == "__main__":
    # parsing commandline arguments.
    parser = get_commandline_parser()
    args = parser.parse_args()
    stlFilename = args.stl_filepath
    diameter = float(args.diameter)
    step = float(args.step_size)
    feed = float(args.feed_rate)
    calculate_time = args.calculate_time
    # generate heightmaps for the given stl file.
    try:
        command = ["./scripts/heightmapGenerator", stlFilename]
        generatorOutput = subprocess.check_output(command)
    except CalledProcessError:
        print "Error: Heightmap generator failed"
        exit(-1)

    # global variable to provide common access to volumetric model for process
    STATE = json.loads(generatorOutput)
    MODEL = get_model(STATE)
    SCALE = STATE["scale"]
    MODEL_LENGTH = STATE["model"]["length"]
    MODEL_WIDTH = STATE["model"]["width"]
    MODEL_HEIGHT = STATE["model"]["height"]
    # generate instructions for the generated heightmap.
    TIME_LOG, INSTRUCTIONS = generate_instructions(diameter, step, feed,
                                                   STATE)
    print INSTRUCTIONS

    if calculate_time == True:
        print TIME_LOG
