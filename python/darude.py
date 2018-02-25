# This is a really wonky controller, please do not look at this code.
from serial import *
from tkinter import *

serialPort = "/dev/ttyUSB0"
baudRate = 9600
ser = Serial(serialPort , baudRate, timeout=0, writeTimeout=0)
root = Tk()
root.wm_title("Reading Serial")

def red_led():
    ser.write(b'\x01')
def green_led():
    ser.write(b'\x02')
def blue_led():
    ser.write(b'\x03')

scrollbar = Scrollbar(root)
scrollbar.pack(side=RIGHT, fill=Y)
R = Button(root, text ="Red", bg="#ff0000", activebackground="#ff0000", command = red_led)
G = Button(root, text ="Green", bg="#00ff00", activebackground="#00ff00", command = green_led)
B = Button(root, text ="Blue", bg="#00FFFF", activebackground="#00FFFF", command = blue_led)
R.pack()
G.pack()
B.pack()
log = Text ( root, width=30, height=30, takefocus=0)
log.pack()

log.config(yscrollcommand=scrollbar.set)
scrollbar.config(command=log.yview)

serBuffer = ""
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
            global serBuffer
            serBuffer = serBuffer + c.decode('ascii')
        if (serBuffer != "" and ser.inWaiting() == 0):
            log.insert(END, serBuffer)
            print(serBuffer)
            serBuffer = ""
        if (ser.inWaiting() == 0):
          break

    root.after(10, readSerial)
root.after(100, readSerial)

root.mainloop()
