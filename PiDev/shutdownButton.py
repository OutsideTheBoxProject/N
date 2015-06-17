import RPi.GPIO as GPIO
import time
import os

GPIO.setmode(GPIO.BCM)
GPIO.setup(17,GPIO.IN, pull_up_down=GPIO.PUD_DOWN)

shuttingdown = False

while not shuttingdown:
	if (GPIO.input(17)):
		os.system("shutdown -h now")
		shuttingdown = True
	time.sleep(0.2)
