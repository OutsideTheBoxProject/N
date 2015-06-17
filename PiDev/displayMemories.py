# global imports
import os, random, numpy, time, math, collections
import pygame
from pygame.locals import *
import PIL
from PIL import Image
from signal import alarm, signal, SIGALRM, SIGKILL
import RPi.GPIO as GPIO

# local imports
import constants as con
import logging as log
import fileImport as fi

# global variables
running = 0
pause = False
screen = None
importDisplay = True

# implementation

# get a sorted array of folder contents
def get_dir_content(filedir):
	contents = os.listdir(filedir)
	contents.sort()
	return contents
	
# get an array of lines of a text file
def get_lines(textfile):
	with open(textfile) as f:
		return f.readlines()
	
# create a dictionary holding the bpm 	
def get_bpm_dictionary(foldername):
	bpms = {}
	pulselines = get_lines(foldername + con.PULSELOG)
	lastStamp = ""
	pulses = []
	for line in pulselines:
		data = line.split(",")
		if not len(lastStamp) == 0 and not lastStamp == data[0]:
			bpms[lastStamp] = numpy.mean(pulses)
			pulses = []
		lastStamp = data[0]	
		pulses.append(int(data[1]))
	bpms = collections.OrderedDict(sorted(bpms.items()))
	return bpms
	

# get the correct bpm value for the picture
def get_pulsedata(pics, i, bpms):
	if len(bpms) == 0:
		return 0.0
	beats = []
	stamp = pics.keys()[i]
	if i == 0:
		if bpms.keys()[i] > stamp :
			return float(bpms[bpms.keys()[i]])
		else:
			for key in bpms.keys():
				if key > stamp:
					return float(numpy.mean(beats))
				else:
					beats.append(bpms[key])
	else:
		prevStamp = pics.keys()[i-1]
		potBpm = 0
		for key in bpms.keys():
			if key > prevStamp and stamp >= key:
				beats.append(bpms[key])
		if len(beats) > 0:
			return float(numpy.mean(beats))
		else:
			for key in bpms.keys():
				if stamp > key:
					potBpm = bpms[key]
				else:
					if potBpm > 0:
						return float(potBpm)
					else:
						return float(bpms[bpms.keys()[0]])
	return float(bpms[bpms.keys()[-1]])		

		
# displays bpm data for now
def display_bpms(pics, i, bpms):
	bpm = get_pulsedata(pics, i, bpms)
	font = pygame.font.Font(con.FONT, 25)
	bpmText = font.render("{0:}".format(int(round(bpm,2))) + " bpm", 1, con.LINECOLOUR)
	screen.blit(bpmText, (con.SCREENWIDTH/15, 10))
	for y in range(60, con.SCREENHEIGHT):
		x = int((con.SCREENWIDTH/9) + con.AMPLITUDE * math.sin((bpm * .05) * ((float(y)/con.SCREENHEIGHT) * (2*math.pi) + (con.SPEED * time.time()))))
		screen.set_at((x,y), con.LINECOLOUR)
		screen.set_at((x,y+1), con.LINECOLOUR)
		screen.set_at((x,y-1), con.LINECOLOUR)
		screen.set_at((x+1,y), con.LINECOLOUR)
		screen.set_at((x-1,y), con.LINECOLOUR)
	pygame.display.flip()
	
	


# actually show the pics
def perform_pic_loop(foldername, pics):
	global running, screen, pause
	i = 0
	bpms = get_bpm_dictionary(foldername)
	while True:
		# display pic
		if (running == 0 or (pygame.time.get_ticks() - running) > con.WAITTIME) and not pause:
			screen.fill(con.BACKGROUNDCOLOUR)
			screen.blit(pygame.image.load(foldername + pics[pics.keys()[i]]), ((con.SCREENWIDTH - con.PICTUREWIDTH), 0))
			pygame.display.flip()
			running = pygame.time.get_ticks()
			display_bpms(pics, i, bpms)
			i = i + 1
			if i >= len(pics):
				break 
			#check_import(False)

		
		for event in pygame.event.get():	
			#exit
			if event.type == pygame.KEYDOWN:
				if event.key == pygame.K_ESCAPE:
					print "Goodbye."
					exit()
		
		toggle = True			
		while (GPIO.input(4)):
			if toggle: 
				if pause:
					pause = False
				else:
					pause = True
				toggle = False

# because sorting might not be enough,
# we need to play the pictures in the right order.
# returns a mapping of timestamps and filenames
def get_order_of_pictures(foldername):
	piclog = {}
	loglines = get_lines(foldername + con.PICLOG)
	for line in loglines:
		data = line.split(",")
		piclog[data[0]] = data[1].strip().upper()
	piclog = collections.OrderedDict(sorted(piclog.items()))
	return piclog


# play only recent files
def play_recent_files(filedir):
	recent = get_dir_content(filedir)[-1]
	rawPics = get_order_of_pictures(filedir + recent + "/")
	perform_pic_loop(filedir + recent + "/", rawPics)
	


# loop over all directories and files
def play_all_files(filedir):
	folders = get_dir_content(filedir)
	pics = []
	for folder in folders:
		rawPics = get_order_of_pictures(filedir + folder + "/")
		perform_pic_loop(filedir + folder + "/", rawPics)

# returns the correct number of days happened in that year up to the month given
def get_days_for_month(month):
	days = 0
	while month > 1:
		if month == 2 or month == 4 or month == 6 or month == 8 or month == 9 or month == 11:
			days = days + 31
		elif month == 3:
			days = days + 28
		elif month == 5 or month == 7 or month == 10 or month == 12:
			days = days + 30
		month = month - 1
	return days
	
	
# takes a date string Y-m-d and returns the number of days since 1.1.2000
def get_days(datestring):
	dateparts = datestring.split("-")
	years = (int(dateparts[0]) - 2000)
	# rough estimation of leap years
	leapyears = int(math.ceil(years/4))
	months = int(dateparts[1])
	days = int(dateparts[2])
	return years * 365 + get_days_for_month(months) + days + leapyears

# returns a dictionary containing the sweep data
def get_sweep_data():
	sweeplines = get_lines(con.SWEEPFILE)
	sweeps = {}
	for line in sweeplines:
		linecontent = line.split(", ")
		sweeps[linecontent[0]] = [int(linecontent[1]), get_days(linecontent[2])]
	return sweeps


# actually sweep pictures according to half time according to sweep mode
def perform_sweep():
	sweepdata = get_sweep_data()
	if len(sweepdata) == 0:
		return
	todaydays = get_days(time.strftime("%Y-%m-%d"))
	for key in sweepdata.keys():
		to = int((todaydays - sweepdata[key][1])//con.SWEEPTIME)
		maxpics = int( (1/(2**to)) * sweepdata[key][0])
		curpics = get_dir_content(con.PICS + key)
		picpics = []
		for potpic in curpics:
			if "JPG" in potpic:
				picpics.append(potpic)
		todelete = len(picpics) - maxpics
		if todelete > 0:
			deletefiles = []
			indices = random.sample(range(0,len(picpics)), todelete)
			for ind in indices:
				deletefiles.append(picpics[ind])
			if con.SWEEPMODE == con.PRINT:
				for f in deletefiles:
					print "would remove file " + key + f
			elif con.SWEEPMODE == con.DELETE:
				for f in deletefiles:
					os.remove(key + f)
			elif con.SWEEPMODE == con.MOVE:
				mfolder = con.MOVEFOLDER + key.split("/")[2]
				for f in deletefiles:
					if not os.path.exists(mfolder):
						os.makedirs(mfolder)
					os.rename(key + f, mfolder + "/" + f)
			if con.LOGGING:
				log.log_picture_deletion(key + f)
			if len(get_dir_content(key)) == 0:
				os.rmdir(key)

# writes sweep information into the sweepfile
def write_sweep(folder, file_number, timestamp):	
	with open(con.SWEEPFILE, "a") as f:
		f.write(str(folder) + ", " + str(file_number) + ", " + timestamp + "\n" )

# updates the sweepfile so we know what to do later
def update_sweepfile():
	folders = get_dir_content(con.PICS)
	folder = ""
	if len(folders) > 0:
		folder = get_dir_content(con.PICS)[-1]
		contents = get_dir_content(con.PICS + folder)
		if len(contents) == 0:
			os.rmdir(con.PICS + folder)
		else: 
			file_number = len(contents) - 2
			timestamp = log.get_date_timestamp()
			write_sweep(folder, file_number, timestamp)
	

# display that we import files
def display_import_files():
	global screen
	screen.fill(con.BACKGROUNDCOLOUR)	
	font = pygame.font.Font(con.FONT, 50)
	bpmText = font.render("Bitte kurz warten,", 1, con.IMPORTCOLOUR)
	screen.blit(bpmText, (con.SCREENWIDTH/3.1, con.SCREENHEIGHT/2 - 125))
	bpmText = font.render("ich hole mir neue Bilder", 1, con.IMPORTCOLOUR)
	screen.blit(bpmText, (con.SCREENWIDTH/3.55, con.SCREENHEIGHT/2 - 25))
	bpmText = font.render("aus der Denkmaschine.", 1, con.IMPORTCOLOUR)
	screen.blit(bpmText, (con.SCREENWIDTH/3.5, con.SCREENHEIGHT/2 + 75))
	pygame.display.flip()
	for event in pygame.event.get():
		#exit
		if event.type == pygame.KEYDOWN:
			if event.key == pygame.K_ESCAPE:
				print "Goodbye."
				exit()
	
# just so it's not confusing during timeout	
def display_checking_import():
	global screen
	screen.fill(con.BACKGROUNDCOLOUR)	
	font = pygame.font.Font(con.FONT, 50)
	bpmText = font.render("Bitte kurz warten,", 1, con.IMPORTCOLOUR)
	screen.blit(bpmText, (con.SCREENWIDTH/3.1, con.SCREENHEIGHT/2 - 125))
	bpmText = font.render("ich schaue nach, ob ich", 1, con.IMPORTCOLOUR)
	screen.blit(bpmText, (con.SCREENWIDTH/3.5, con.SCREENHEIGHT/2 - 25))
	bpmText = font.render("neue Bilder finden kann.", 1, con.IMPORTCOLOUR)
	screen.blit(bpmText, (con.SCREENWIDTH/3.5, con.SCREENHEIGHT/2 + 75))
	pygame.display.flip()
	for event in pygame.event.get():
		#exit
		if event.type == pygame.KEYDOWN:
			if event.key == pygame.K_ESCAPE:
				print "Goodbye."
				exit()

# checks for file import and does so, if there is a connection
def check_import(withDisplay):
	global importDisplay
	if withDisplay:
		display_checking_import()
	numFolders = len(get_dir_content(con.PICS))
	if fi.test_connection():
		if con.LOGGING:
			log.log_data_transfer_start()
		if withDisplay:	
			display_import_files()
		fi.get_files_from_headband()
		update_sweepfile()
		importDisplay = False
		if con.LOGGING and numFolders < len(get_dir_content(con.PICS)):
			log.log_data_transfer_finish()


def display_no_pictures():
    global screen
    screen.fill(con.BACKGROUNDCOLOUR)
    font = pygame.font.Font(con.FONT, 50)
    bpmText = font.render("Ich habe noch keine Bilder.", 1, con.IMPORTCOLOUR)
    screen.blit(bpmText, (con.SCREENWIDTH/4, con.SCREENHEIGHT/2 - 125))
    bpmText = font.render("Bitte mach doch erst ein paar", 1, con.IMPORTCOLOUR)
    screen.blit(bpmText, (con.SCREENWIDTH/4.5	, con.SCREENHEIGHT/2 - 25))
    bpmText = font.render("mit deiner Denkmaschine.", 1, con.IMPORTCOLOUR)
    screen.blit(bpmText, (con.SCREENWIDTH/3.8, con.SCREENHEIGHT/2 + 75))
    pygame.display.flip()
    for event in pygame.event.get():
		if event.type == pygame.KEYDOWN:
			if event.key == pygame.K_ESCAPE:
				print "Goodbye."
				exit()

# this is a bloody hack. 
def init_pygame():
	global screen
	class Alarm(Exception):
		pass
	def alarm_handler(signum, frame):
		raise Alarm
	signal(SIGALRM, alarm_handler)
	alarm(3)
	try:
		pygame.init()
		screen = pygame.display.set_mode((con.SCREENWIDTH, con.SCREENHEIGHT), pygame.FULLSCREEN)
		alarm(0)
	except Alarm:
		raise KeyboardInterrupt
	
	screen.fill((255, 255, 255))
	pygame.display.flip()


# main function
def main():
	global importDisplay
	GPIO.setmode(GPIO.BCM)
	GPIO.setup(4,GPIO.IN, pull_up_down=GPIO.PUD_DOWN)	
	init_pygame()
	log.append_line(con.STATIONLOG, log.get_line(str(type(screen))))
	if con.LOGGING:
		log.log_start_station()
	check_import(importDisplay)
	if len(get_dir_content(con.PICS)) > 0:
		play_recent_files(con.PICS)
		perform_sweep()
	else:
		log.log_no_pictures()
	while True:
		if con.LOGGING:
			log.log_picture_cycle()
		if len(get_dir_content(con.PICS)) > 0:
			play_all_files(con.PICS)
			check_import(True)
		else:
			display_no_pictures()
			check_import(False)
