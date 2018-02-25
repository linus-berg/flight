# This is a really wonky controller, please do not look at this code.
from serial import *
from tkinter import *

ser = Serial("/dev/ttyUSB0" , 9600, timeout=0, writeTimeout=0)
root = Tk()
root.wm_title("Flight Control Centre")

def red_led():
    ser.write(b'\x01')
def green_led():
    ser.write(b'\x02')
def blue_led():
    ser.write(b'\x03')

R = Button(root, text ="Red", bg="#ff0000", activebackground="#ff0000", command = red_led)
G = Button(root, text ="Green", bg="#00ff00", activebackground="#00ff00", command = green_led)
B = Button(root, text ="Blue", bg="#00FFFF", activebackground="#00FFFF", command = blue_led)
R.grid(row=0, column=0)
G.grid(row=0, column=1)
B.grid(row=0, column=2)

def readSerial():
    while True:
        if (ser.inWaiting() > 0):
            c = ser.read(ser.inWaiting())
            print(c)
            if (c == b'\x01'):
                R.configure(bg = "#696969", activebackground="#696969")
            if (c == b'\x11'):
                R.configure(bg = "#ff0000", activebackground="#ff0000")
            if (c == b'\x02'):
                G.configure(bg = "#696969", activebackground="#696969")
            if (c == b'\x12'):
                G.configure(bg = "#00ff00", activebackground="#00ff00")
            if (c == b'\x03'):
                B.configure(bg = "#696969", activebackground="#696969")
            if (c == b'\x13'):
                B.configure(bg = "#00ffff", activebackground="#00ffff")
        if (ser.inWaiting() == 0):
          break
    root.after(10, readSerial)
root.after(100, readSerial)

root.mainloop()
