"""
This python script starts a simple gui to change settings on the controller.
For this purpase I used the tkinter-modul
"""

# imports
from functools import partial
from os import read, terminal_size
import sys
import glob
from tkinter.constants import COMMAND
import serial
from serial import SerialException
import tkinter
from tkinter import Label, Menu, ttk
from time import sleep

# create tkinter window

root = tkinter.Tk()
root.title("Controller Settings")

# serial communication
def serial_ports():
    """ Lists serial port names

        :raises EnvironmentError:
            On unsupported or unknown platforms
        :returns:
            A list of the serial ports available on the system
    """
    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i + 1) for i in range(256)]
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        # this excludes your current terminal "/dev/tty"
        ports = glob.glob('/dev/tty[A-Za-z]*')
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')
    else:
        raise EnvironmentError('Unsupported platform')
	
    result = []
    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass
    return result

ser = serial.Serial()       # serial port object

serial_ports = serial_ports()       # list of all ports
port = serial_ports[-1]             # selected port

def set_port(p):
    port = p
    ser.port = port

serial_bauds = [4800, 9600, 19200, 115200]               # list of possible bauds
baud = 9600                         # selected baud

def set_baud(b):
    baud = b
    ser.baudrate = baud

set_port(port)
set_baud(baud)

last_message = tkinter.StringVar()


# changes the settings
def change_setting(setting):
    ser.open()
    if(ser.is_open == True):
        input = "none"
        
        counter = 0
        while(input != setting and counter < 5):
            ser.write(setting.encode('utf-8') + b'\n')
            ser.flush()

            input = str(ser.readline(100))

            input = input.removeprefix("b")
            input = input.replace("\'", "")
            input = input.replace("\\r", "")
            input = input.replace("\\n", "")

            print(str(counter) + ": " + input)

            counter += 1
            sleep(1)

        last_message.set(input)
        ser.close()
    else:
        last_message.set("ERROR Couldn't open port " + port + " at " + str(baud))

# partial functions for tkinter gui and changing settings
com_mode_controller         = partial(change_setting, "com_mode=controller")
com_mode_serial             = partial(change_setting, "com_mode=serial")
sending_mode_continuous     = partial(change_setting, "sending_mode=continuous")
sending_mode_on_event       = partial(change_setting, "sending_mode=onEvent")
joystick_mode_digital    = partial(change_setting, "joystick_mode=digital")
joystick_mode_analog     = partial(change_setting, "joystick_mode=analog")

# menubar
menubar = tkinter.Menu(root)

# program menu
prog_menu = Menu(menubar, tearoff=0)
prog_menu.add_command(label="Exit", command=exit)
menubar.add_cascade(label="Program", menu=prog_menu)

# select port menu
port_menu = Menu(menubar, tearoff=0)
selected_port = tkinter.StringVar(root, port)
for i in serial_ports:
    # add avery found serial port with a label and a partial function to set_port
    port_menu.add_radiobutton(label=str(i), command=partial(set_port, i), variable=selected_port)
menubar.add_cascade(label="Port", menu=port_menu)

# select baud menu
baud_menu = Menu(menubar, tearoff=0)
selected_baud = tkinter.StringVar(root, baud)
for i in serial_bauds:
    # add avery found serial port with a label and a partial function to set_port
    baud_menu.add_radiobutton(label=str(i), command=partial(set_baud, i), variable=selected_baud)
menubar.add_cascade(label="Baudrate", menu=baud_menu)

root.config(menu=menubar)

# add settings
# communication mode
label_com = tkinter.Label(text="Communication Mode:")
label_com.grid(row=1, column=1, columnspan=2, sticky='we')
button_com_con = tkinter.Button(text="Controller", command=com_mode_controller, padx=5)
button_com_con.grid(row=2, column=1, sticky='we')
button_com_ser = tkinter.Button(text="Serial", command=com_mode_serial, padx=5)
button_com_ser.grid(row=2, column=2, sticky='we')

# sending mode
label_sending = tkinter.Label(text="Sending mode:")
label_sending.grid(row=3, column=1, columnspan=2, sticky='we')
button_sending_cont = tkinter.Button(text="Continuous", command=sending_mode_continuous)
button_sending_cont.grid(row=4, column=1, sticky='we')
button_sending_event = tkinter.Button(text="on event", command=sending_mode_on_event)
button_sending_event.grid(row=4, column=2, sticky='we')

# joystick mode
label_joystick = tkinter.Label(text="Joystick mode:")
label_joystick.grid(row=5, column=1, columnspan=2, sticky='we')
button_joystick_analog = tkinter.Button(text="Analog", command=joystick_mode_analog)
button_joystick_analog.grid(row=6, column=1, sticky='we')
button_joystick_digital = tkinter.Button(text="Digital", command=joystick_mode_digital)
button_joystick_digital.grid(row=6, column=2, sticky='we')

# terminal
label_terminal_title = tkinter.Label(text="Terminal:")
label_terminal_title.grid(row=7, column=1, columnspan=2, sticky='we')
label_terminal = tkinter.Label(textvariable=last_message, fg="green2", bg="black")
label_terminal.grid(row=8, column=1, columnspan=2, sticky='w')

# spacer
label_tab = tkinter.Label(text="\t\t\t\t")
label_tab.grid(row=9, column=1, columnspan=2, sticky='we')

root.resizable(False, False)
root.mainloop()