#!/usr/bin/env python

import sys
try:
 	import pygtk
  	pygtk.require("2.0")
except:
  	pass
try:
	import gtk
  	import gtk.glade
except:
	sys.exit(1)


# Core Routine
#
# Defines initial variables and conditions
class Onus:
	def __init__(self):
		self.gladefile = "onus.glade"  
		self.wTree = gtk.glade.XML(self.gladefile, "mainWindow")


# Execute Main Program
#
# Calls the initial routine, Onus(), and displays the program window.	
if __name__ == "__main__":
	onus = Onus()
	gtk.main()
