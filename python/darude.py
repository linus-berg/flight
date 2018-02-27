# This is a really wonky controller, please do not look at this code.
from serial import *
from tkinter import *
import sys
import spotipy
import spotipy.util as util

# Serial Setup
ser = Serial("/dev/ttyUSB0" , 9600, timeout=0, writeTimeout=0)

# Tkinter Setup
root = Tk()
root.wm_title("Flight Control Tower")

# Spotify connection
scope = 'user-read-currently-playing user-modify-playback-state'
token = util.prompt_for_user_token('dalk', scope)
if token:
  sp = spotipy.Spotify(auth=token)
  sp.trace = False

def mcu_led(num):
  ser.write(num)

R = Button(root, text ="Red", bg="#ff0000", activebackground="#ff0000",
           command = lambda: mcu_led(b'\x01'))
G = Button(root, text ="Green", bg="#00ff00", activebackground="#00ff00",
           command = lambda: mcu_led(b'\x02'))
B = Button(root, text ="Blue", bg="#00FFFF", activebackground="#00FFFF",
           command = lambda: mcu_led(b'\x03'))
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
      if (c == b'\x50'):
        if (sp.currently_playing()['is_playing']):
          sp.pause_playback()
        else:
          sp.start_playback()
      if (c == b'\x51'):
        sp.previous_track()
      if (c == b'\x52'):
        sp.next_track()
    if (ser.inWaiting() == 0):
      break
  root.after(10, readSerial)

root.after(100, readSerial)
root.mainloop()
