import matplotlib.pyplot as plt
import os

path="./data/"
FILES=os.listdir(path)

t = 0
time = []
step_count = []

# Get the Data
for file in FILES:
	filename = path + file
	infile = open(filename,"r")
	data_section = 0
	while 1:
		line = infile.readline()
		if not line:
			break
		moment = line.split(",")
		time.append( t )
		t += 1
		step_count.append( moment[0] )
plt.plot(time, step_count)
plt.axis([0, 2000, 0, 500])
plt.show()
