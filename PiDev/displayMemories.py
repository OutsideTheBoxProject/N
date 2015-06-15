# global imports
import os, random, numpy, time, math
import pygame
from pygame.locals import *
import PIL
from PIL import Image

# local imports
import constants as con
import logging as log
import fileImport as fi

# global variables
running = 0
pause = False
imported = False

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
	
		
# temporary solution, but will display some bpm data for now
def display_bpms():
	# fake some random data 
	pulsedata = random.sample(range(50, 220), random.randint(1,30))
	bpm = numpy.mean(pulsedata)
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
def perform_pic_loop(pics):
	global running, screen, pause
	i = 0
	while True:
		# display pic
		if (running == 0 or (pygame.time.get_ticks() - running) > con.WAITTIME) and not pause:
			screen.fill(con.BACKGROUNDCOLOUR)
			screen.blit(pygame.image.load(pics[i]), ((con.SCREENWIDTH - con.PICTUREWIDTH), 0))
			pygame.display.flip()
			running = pygame.time.get_ticks()
			display_bpms()
			i = i + 1
			if i >= len(pics):
				break 
		
		for event in pygame.event.get():	
			#exit
			if event.type == pygame.KEYDOWN:
				if event.key == pygame.K_ESCAPE:
					print "Goodbye."
					exit()
					
				# pause button until we have a real one
				if event.key == pygame.K_p:
					# do stuff
					if pause:
						pause = False
					else:
						pause = True
	


# play only recent files
def play_recent_files(filedir):
	recent = get_dir_content(filedir)[-1]
	rawPics = get_dir_content(filedir + recent)
	pics = []
	for pic in rawPics:
		if "JPG" in pic:
			pics.append(filedir + recent + "/" + pic)
	perform_pic_loop(pics)
	


# loop over all directories and files
def play_all_files(filedir):
	folders = get_dir_content(filedir)
	pics = []
	for folder in folders:
		rawPics = get_dir_content(filedir + folder)
		for pic in rawPics:
			if "JPG" in pic:
				pics.append(filedir + folder + "/" + pic)
	perform_pic_loop(pics)

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
		print linecontent
		print len(linecontent)
		sweeps[linecontent[0]] = [int(linecontent[1]), get_days(linecontent[2])]
	return sweeps


# actually sweep pictures according to half time according to sweep mode
def perform_sweep():
	sweepdata = get_sweep_data()
	todaydays = get_days(time.strftime("%Y-%m-%d"))
	for key in sweepdata.keys():
		to = int((todaydays - sweepdata[key][1])//con.SWEEPTIME)
		maxpics = int( (1/(2**to)) * sweepdata[key][0])
		curpics = get_dir_content(key)
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
	folder = get_dir_content(con.PICS)[-1]
	contents = get_dir_content(con.PICS + folder)
	file_number = len(content) - 2
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

# checks for file import and does so, if there is a connection
def check_import():
	global imported
	if fi.test_connection():
		if con.LOGGING:
			log.log_data_transfer_start()
		display_import_files()
		fi.get_files_from_headband()
		update_sweepfile()
		imported = True
		if con.LOGGING:
			log.log_data_transfer_finish()	

# main function
def main():
	if con.LOGGING:
		log.log_start_station()
	check_import()
	#play_recent_files(con.PICS)
	perform_sweep()
	while True:
		if con.LOGGING:
			log.log_picture_cycle()
		if not imported:
			check_import()
		play_all_files(con.PICS)
		
# initialise screen
pygame.init()
screen = pygame.display.set_mode((con.SCREENWIDTH, con.SCREENHEIGHT), pygame.FULLSCREEN)
screen.fill((255, 255, 255))
pygame.display.flip()
		
# call to the main function
main()
