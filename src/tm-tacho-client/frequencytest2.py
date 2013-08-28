import threading
import time
import RPi.GPIO as GPIO
import os
import sys

class gpioThread (threading.Thread):
	def __init__(self, pin):
		self.pin = pin
		self.freq = 1.0
		self.running = True
		GPIO.setmode(GPIO.BCM)
		GPIO.setup(self.pin, GPIO.OUT)
		self.high = 0
		threading.Thread.__init__(self)
	def run(self):
		while self.running:
			if self.freq < 1.0:
				self.freq = 1.0

			GPIO.output(self.pin, True)			
			time.sleep(1.0/self.freq/2.0)
			GPIO.output(self.pin, False)
			time.sleep(1.0/self.freq/2.0)

			self.high = self.high + 1

def main():
	gpiothread = gpioThread(23)
	gpiothread.setDaemon(True)
	gpiothread.start()
	freq = 1.0

	def killAll():
		gpiothread.running = False

	while(1):
		print freq
		freq = float(raw_input("> "))
		gpiothread.freq = freq

if __name__ == '__main__': main()
