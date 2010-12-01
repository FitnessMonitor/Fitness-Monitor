process.py is a Python program you run from the terminal.
It requires python-matplotlib
It will look for all the files in data, then load each line into a python list (array) and graph it.


Expanding it to graph multiple lines:
	Create a new line like the following:
		next_type.append( moment[1] )
	Then modifiy the plot like to look like this:
		plt.plot(time, step_count, time, next_type)
	The data in the files should be seperated by a comma, noting the following command:
		moment = line.split(",")
