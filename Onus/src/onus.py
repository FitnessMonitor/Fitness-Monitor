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
import matplotlib   
matplotlib.use('GTK')   
from matplotlib.figure import Figure   
from matplotlib.axes import Subplot
from matplotlib.backends.backend_gtk import FigureCanvasGTK, NavigationToolbar   
from matplotlib.numerix import arange, sin, pi   

import sys, os, serial, threading

EXITCHARCTER = '\x1d'   # GS/CTRL+]
MENUCHARACTER = '\x14'  # Menu: CTRL+T


def key_description(character):
    """generate a readable description for a key"""
    ascii_code = ord(character)
    if ascii_code < 32:
        return 'Ctrl+%c' % (ord('@') + ascii_code)
    else:
        return repr(character)

# help text, starts with blank line! it's a function so that the current values
# for the shortcut keys is used and not the value at program start
def get_help_text():
    return """
--- pySerial (%(version)s) - miniterm - help
---
--- %(exit)-8s Exit program
--- %(menu)-8s Menu escape key, followed by:
--- Menu keys:
---       %(itself)-8s Send the menu character itself to remote
---       %(exchar)-8s Send the exit character to remote
---       %(info)-8s Show info
---       %(upload)-8s Upload file (prompt will be shown)
--- Toggles:
---       %(rts)s  RTS          %(echo)s  local echo
---       %(dtr)s  DTR          %(break)s  BREAK
---       %(lfm)s  line feed    %(repr)s  Cycle repr mode
---
--- Port settings (%(menu)s followed by the following):
--- 7 8           set data bits
--- n e o s m     change parity (None, Even, Odd, Space, Mark)
--- 1 2 3         set stop bits (1, 2, 1.5)
--- b             change baud rate
--- x X           disable/enable software flow control
--- r R           disable/enable hardware flow control
""" % {
    'version': getattr(serial, 'VERSION', 'unkown'),
    'exit': key_description(EXITCHARCTER),
    'menu': key_description(MENUCHARACTER),
    'rts': key_description('\x12'),
    'repr': key_description('\x01'),
    'dtr': key_description('\x04'),
    'lfm': key_description('\x0c'),
    'break': key_description('\x02'),
    'echo': key_description('\x05'),
    'info': key_description('\x09'),
    'upload': key_description('\x15'),
    'itself': key_description(MENUCHARACTER),
    'exchar': key_description(EXITCHARCTER),
}

# first choose a platform dependant way to read single characters from the console
global console

if os.name == 'nt':
    import msvcrt
    class Console:
        def __init__(self):
            pass

        def setup(self):
            pass    # Do nothing for 'nt'

        def cleanup(self):
            pass    # Do nothing for 'nt'

        def getkey(self):
            while 1:
                z = msvcrt.getch()
                if z == '\0' or z == '\xe0':    # functions keys
                    msvcrt.getch()
                else:
                    if z == '\r':
                        return '\n'
                    return z

    console = Console()

elif os.name == 'posix':
    import termios, sys, os
    class Console:
        def __init__(self):
            self.fd = sys.stdin.fileno()

        def setup(self):
            self.old = termios.tcgetattr(self.fd)
            new = termios.tcgetattr(self.fd)
            new[3] = new[3] & ~termios.ICANON & ~termios.ECHO & ~termios.ISIG
            new[6][termios.VMIN] = 1
            new[6][termios.VTIME] = 0
            termios.tcsetattr(self.fd, termios.TCSANOW, new)

        def getkey(self):
            c = os.read(self.fd, 1)
            return c

        def cleanup(self):
            termios.tcsetattr(self.fd, termios.TCSAFLUSH, self.old)

    console = Console()

    def cleanup_console():
        console.cleanup()

    console.setup()
    sys.exitfunc = cleanup_console      #terminal modes have to be restored on exit...

else:
    raise "Sorry no implementation for your platform (%s) available." % sys.platform


CONVERT_CRLF = 2
CONVERT_CR   = 1
CONVERT_LF   = 0
NEWLINE_CONVERISON_MAP = ('\n', '\r', '\r\n')
LF_MODES = ('LF', 'CR', 'CR/LF')

REPR_MODES = ('raw', 'some control', 'all control', 'hex')

class Miniterm:
    def __init__(self, port, baudrate, parity, rtscts, xonxoff, echo=False, convert_outgoing=CONVERT_CRLF, repr_mode=0):
        try:
            self.serial = serial.serial_for_url(port, baudrate, parity=parity, rtscts=rtscts, xonxoff=xonxoff, timeout=1)
        except AttributeError:
            # happens when the installed pyserial is older than 2.5. use the
            # Serial class directly then.
            self.serial = serial.Serial(port, baudrate, parity=parity, rtscts=rtscts, xonxoff=xonxoff, timeout=1)
        self.echo = echo
        self.repr_mode = repr_mode
        self.convert_outgoing = convert_outgoing
        self.newline = NEWLINE_CONVERISON_MAP[self.convert_outgoing]
        self.dtr_state = True
        self.rts_state = True
        self.break_state = False

    def start(self):
        self.alive = True
        # start serial->console thread
        self.receiver_thread = threading.Thread(target=self.reader)
        self.receiver_thread.setDaemon(1)
        self.receiver_thread.start()
        # enter console->serial loop
        self.transmitter_thread = threading.Thread(target=self.writer)
        self.transmitter_thread.setDaemon(1)
        self.transmitter_thread.start()

    def stop(self):
        self.alive = False

    def join(self, transmit_only=False):
        self.transmitter_thread.join()
        if not transmit_only:
            self.receiver_thread.join()

    def dump_port_settings(self):
        sys.stderr.write("\n--- Settings: %s  %s,%s,%s,%s\n" % (
            self.serial.portstr,
            self.serial.baudrate,
            self.serial.bytesize,
            self.serial.parity,
            self.serial.stopbits,
        ))
        sys.stderr.write('--- RTS %s\n' % (self.rts_state and 'active' or 'inactive'))
        sys.stderr.write('--- DTR %s\n' % (self.dtr_state and 'active' or 'inactive'))
        sys.stderr.write('--- BREAK %s\n' % (self.break_state and 'active' or 'inactive'))
        sys.stderr.write('--- software flow control %s\n' % (self.serial.xonxoff and 'active' or 'inactive'))
        sys.stderr.write('--- hardware flow control %s\n' % (self.serial.rtscts and 'active' or 'inactive'))
        sys.stderr.write('--- data escaping: %s\n' % (REPR_MODES[self.repr_mode],))
        sys.stderr.write('--- linefeed: %s\n' % (LF_MODES[self.convert_outgoing],))
        try:
            sys.stderr.write('--- CTS: %s  DSR: %s  RI: %s  CD: %s\n' % (
                (self.serial.getCTS() and 'active' or 'inactive'),
                (self.serial.getDSR() and 'active' or 'inactive'),
                (self.serial.getRI() and 'active' or 'inactive'),
                (self.serial.getCD() and 'active' or 'inactive'),
                ))
        except serial.SerialException:
            # on RFC 2217 ports it can happen to no modem state notification was
            # yet received. ignore this error.
            pass

    def reader(self):
        """loop and copy serial->console"""
        try:
            while self.alive:
                data = self.serial.read(1)

                if self.repr_mode == 0:
                    # direct output, just have to care about newline setting
                    if data == '\r' and self.convert_outgoing == CONVERT_CR:
                        sys.stdout.write('\n')
                    else:
                        sys.stdout.write(data)
                elif self.repr_mode == 1:
                    # escape non-printable, let pass newlines
                    if self.convert_outgoing == CONVERT_CRLF and data in '\r\n':
                        if data == '\n':
                            sys.stdout.write('\n')
                        elif data == '\r':
                            pass
                    elif data == '\n' and self.convert_outgoing == CONVERT_LF:
                        sys.stdout.write('\n')
                    elif data == '\r' and self.convert_outgoing == CONVERT_CR:
                        sys.stdout.write('\n')
                    else:
                        sys.stdout.write(repr(data)[1:-1])
                elif self.repr_mode == 2:
                    # escape all non-printable, including newline
                    sys.stdout.write(repr(data)[1:-1])
                elif self.repr_mode == 3:
                    # escape everything (hexdump)
                    for character in data:
                        sys.stdout.write("%s " % character.encode('hex'))
                sys.stdout.flush()
        except serial.SerialException, e:
            self.alive = False
            # would be nice if the console reader could be interruptted at this
            # point...
            raise


    def writer(self):
        """loop and copy console->serial until EXITCHARCTER character is
           found. when MENUCHARACTER is found, interpret the next key
           locally.
        """
        menu_active = False
        try:
            while self.alive:
                try:
                    c = console.getkey()
                except KeyboardInterrupt:
                    c = '\x03'
                if menu_active:
                    if c == MENUCHARACTER or c == EXITCHARCTER: # Menu character again/exit char -> send itself
                        self.serial.write(c)                    # send character
                        if self.echo:
                            sys.stdout.write(c)
                    elif c == '\x15':                       # CTRL+U -> upload file
                        sys.stderr.write('\n--- File to upload: ')
                        sys.stderr.flush()
                        console.cleanup()
                        filename = sys.stdin.readline().rstrip('\r\n')
                        if filename:
                            try:
                                file = open(filename, 'r')
                                sys.stderr.write('--- Sending file %s ---\n' % filename)
                                while True:
                                    line = file.readline().rstrip('\r\n')
                                    if not line:
                                        break
                                    self.serial.write(line)
                                    self.serial.write('\r\n')
                                    # Wait for output buffer to drain.
                                    self.serial.flush()
                                    sys.stderr.write('.')   # Progress indicator.
                                sys.stderr.write('\n--- File %s sent ---\n' % filename)
                            except IOError, e:
                                sys.stderr.write('--- ERROR opening file %s: %s ---\n' % (filename, e))
                        console.setup()
                    elif c in '\x08hH?':                    # CTRL+H, h, H, ? -> Show help
                        sys.stderr.write(get_help_text())
                    elif c == '\x12':                       # CTRL+R -> Toggle RTS
                        self.rts_state = not self.rts_state
                        self.serial.setRTS(self.rts_state)
                        sys.stderr.write('--- RTS %s ---\n' % (self.rts_state and 'active' or 'inactive'))
                    elif c == '\x04':                       # CTRL+D -> Toggle DTR
                        self.dtr_state = not self.dtr_state
                        self.serial.setDTR(self.dtr_state)
                        sys.stderr.write('--- DTR %s ---\n' % (self.dtr_state and 'active' or 'inactive'))
                    elif c == '\x02':                       # CTRL+B -> toggle BREAK condition
                        self.break_state = not self.break_state
                        self.serial.setBreak(self.break_state)
                        sys.stderr.write('--- BREAK %s ---\n' % (self.break_state and 'active' or 'inactive'))
                    elif c == '\x05':                       # CTRL+E -> toggle local echo
                        self.echo = not self.echo
                        sys.stderr.write('--- local echo %s ---\n' % (self.echo and 'active' or 'inactive'))
                    elif c == '\x09':                       # CTRL+I -> info
                        self.dump_port_settings()
                    elif c == '\x01':                       # CTRL+A -> cycle escape mode
                        self.repr_mode += 1
                        if self.repr_mode > 3:
                            self.repr_mode = 0
                        sys.stderr.write('--- escape data: %s ---\n' % (
                            REPR_MODES[self.repr_mode],
                        ))
                    elif c == '\x0c':                       # CTRL+L -> cycle linefeed mode
                        self.convert_outgoing += 1
                        if self.convert_outgoing > 2:
                            self.convert_outgoing = 0
                        self.newline = NEWLINE_CONVERISON_MAP[self.convert_outgoing]
                        sys.stderr.write('--- line feed %s ---\n' % (
                            LF_MODES[self.convert_outgoing],
                        ))
                    #~ elif c in 'pP':                         # P -> change port XXX reader thread would exit
                    elif c in 'bB':                         # B -> change baudrate
                        sys.stderr.write('\n--- Baudrate: ')
                        sys.stderr.flush()
                        console.cleanup()
                        backup = self.serial.baudrate
                        try:
                            self.serial.baudrate = int(sys.stdin.readline().strip())
                        except ValueError, e:
                            sys.stderr.write('--- ERROR setting baudrate: %s ---\n' % (e,))
                            self.serial.baudrate = backup
                        else:
                            self.dump_port_settings()
                        console.setup()
                    elif c == '8':                          # 8 -> change to 8 bits
                        self.serial.bytesize = serial.EIGHTBITS
                        self.dump_port_settings()
                    elif c == '7':                          # 7 -> change to 8 bits
                        self.serial.bytesize = serial.SEVENBITS
                        self.dump_port_settings()
                    elif c in 'eE':                         # E -> change to even parity
                        self.serial.parity = serial.PARITY_EVEN
                        self.dump_port_settings()
                    elif c in 'oO':                         # O -> change to odd parity
                        self.serial.parity = serial.PARITY_ODD
                        self.dump_port_settings()
                    elif c in 'mM':                         # M -> change to mark parity
                        self.serial.parity = serial.PARITY_MARK
                        self.dump_port_settings()
                    elif c in 'sS':                         # S -> change to space parity
                        self.serial.parity = serial.PARITY_SPACE
                        self.dump_port_settings()
                    elif c in 'nN':                         # N -> change to no parity
                        self.serial.parity = serial.PARITY_NONE
                        self.dump_port_settings()
                    elif c == '1':                          # 1 -> change to 1 stop bits
                        self.serial.stopbits = serial.STOPBITS_ONE
                        self.dump_port_settings()
                    elif c == '2':                          # 2 -> change to 2 stop bits
                        self.serial.stopbits = serial.STOPBITS_TWO
                        self.dump_port_settings()
                    elif c == '3':                          # 3 -> change to 1.5 stop bits
                        self.serial.stopbits = serial.STOPBITS_ONE_POINT_FIVE
                        self.dump_port_settings()
                    elif c in 'xX':                         # X -> change software flow control
                        self.serial.xonxoff = (c == 'X')
                        self.dump_port_settings()
                    elif c in 'rR':                         # R -> change hardware flow control
                        self.serial.rtscts = (c == 'R')
                        self.dump_port_settings()
                    else:
                        sys.stderr.write('--- unknown menu character %s --\n' % key_description(c))
                    menu_active = False
                elif c == MENUCHARACTER: # next char will be for menu
                    menu_active = True
                elif c == EXITCHARCTER: 
                    self.stop()
                    break                                   # exit app
                elif c == '\n':
                    self.serial.write(self.newline)         # send newline character(s)
                    if self.echo:
                        sys.stdout.write(c)                 # local echo is a real newline in any case
                        sys.stdout.flush()
                else:
                    self.serial.write(c)                    # send character
                    if self.echo:
                        sys.stdout.write(c)
                        sys.stdout.flush()
        except:
            self.alive = False
            raise


class Fetch:
	def __init__(self, port="/dev/ttyUSB0", baudrate=4800, parity='N', rtscts=False, xonxoff=False, echo=False, convert_outgoing=CONVERT_CRLF, repr_mode=0):
		self.port = port
		self.baudrate = baudrate
		self.parity = parity
		self.rtscts = rtscts
		self.xonxoff = xonxoff
		self.echo = echo
		self.start()

	def start(self):
	
		try:
			miniterm = Miniterm(
				self.port,
				self.baudrate,
				self.parity,
				self.rtscts,
				self.xonxoff
			)
		except serial.SerialException, e:
			sys.stderr.write("could not open port %r: %s\n" % (self.port, e))
			sys.exit(1)

		miniterm.start()
		miniterm.join(True)
		#miniterm.join()
        
        
# Core Routine
#
# Defines initial variables and conditions
class Onus:
	def __init__(self):
		#self.gladefile = "onus.glade"
		self.gladefile = "../onus.glade"
		self.wTree = gtk.glade.XML(self.gladefile, "mainWindow")

		FILES = [ "../../0_10_0.TXT" ]

		t = 0
		time = []
		heart_rate = []
		step_count = []
		activity = []

		# Get the Data
		for file in FILES:
			infile = open(file,"r")
			data_section = 0
			while 1:
				line = infile.readline()
				if not line:
					break
				moment = line.split(",")
				time.append( t )
				t += 1
				step_count.append( moment[0] )
				heart_rate.append( moment[1] )
				activity.append( moment[2] )

		self.figure = Figure(figsize=(6,4), dpi=72) 
		self.axis = self.figure.add_subplot(111) 
		self.axis.set_xlabel('Time (minutes)') 
		self.axis.set_ylabel('Heart Rate (bpm)') 
		self.axis.set_title('Activity') 
		self.axis.grid(True)
		self.axis.plot(time, heart_rate, time, step_count, time, activity)
		self.canvas = FigureCanvasGTK(self.figure) # a gtk.DrawingArea 
		self.canvas.show() 
		self.graphview = self.wTree.get_widget("vbox2") 
		self.graphview.pack_start(self.canvas, True, True)
		
		#Create our dictionay and connect it
		dic = { "on_mainWindow_destroy" : gtk.main_quit, "on_connect" : self.on_connect, "on_import" : self.on_import }
		self.wTree.signal_autoconnect(dic)


	def on_connect(self, widget):
		#dan = Fetch()
		#.start()
		self.statusbar1 = self.wTree.get_widget("statusbar1")
		self.statusbar1.push(0, "Connected")

	def on_import(self, widget):
		
		self.statusbar1 = self.wTree.get_widget("statusbar1")
		self.statusbar1.push(0, "Importing")


# Execute Main Program
#
# Calls the initial routine, Onus(), and displays the program window.	
if __name__ == "__main__":
	onus = Onus()
	gtk.main()
