#oggpnson 
#hkhr 

import sys 

folderName = sys.argv[1]

orientations = open("./" + folderName + "/sequence.txt").read().split("->")

for orientation in orientations[:-1]:
	fob = open("./" + folderName + "/" + orientation + ".interim.raw", "r")
	text = fob.read() 

	# text = int(text[::-1], base=2)

	binary_text = []



	# for i in range(0, len(text), 8):
	# 	by = text[i:i+8]
	# 	by = by[::-1]
	# 	by = int(by, 2)
	# 	binary_text.append(by)
		


	for x in text:
		binary_text.append(int(x))



	fob = open("./" + folderName + "/" + orientation + "_8bit.raw", "wb")
	fob.write(bytearray(binary_text))
	fob.close()	

