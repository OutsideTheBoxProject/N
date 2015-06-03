# global imports
import os, random, numpy, time, math
import pygame
from pygame.locals import *
import PIL
from PIL import Image

# local imports
import constants as con

# global variables
running = 0

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
	font = pygame.font.SysFont("monospace", 25)
	bpmText = font.render("{0:.2f}".format(round(bpm,2)) + " bpm", 1, con.LINECOLOUR)
	screen.blit(bpmText, (40, 10))
	prevx = 0
	prevy = 40
	for y in range(60, con.SCREENHEIGHT):
		x = int((con.SCREENWIDTH/9) + con.AMPLITUDE * math.sin((bpm * .1) * ((float(y)/con.SCREENHEIGHT) * (2*math.pi) + (con.SPEED * time.time()))))
		screen.set_at((x,y), con.LINECOLOUR)
		#if not prevx == 0:
		#	pygame.draw.line(screen, con.RED, (prevx, prevy), (x,y), con.LINEWIDTH)
		#prevx = x
		#prevy = y
	pygame.display.flip()
	
	


# actually show the pics
def perform_pic_loop(pics):
	global running, screen
	i = 0
	while True:
		# display pic
		if running == 0 or (pygame.time.get_ticks() - running) > con.WAITTIME:
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
	


# play only recent files
def play_recent_files(filedir):
	recent = get_dir_content(filedir)[-1]
	rawPics = get_dir_content(filedir + recent)
	pics = []
	for pic in rawPics:
		pics.append(filedir + recent + "/" + pic)
	perform_pic_loop(pics)
	


# loop over all directories and files
def play_all_files(filedir):
	folders = get_dir_content(filedir)
	pics = []
	for folder in folders:
		rawPics = get_dir_content(filedir + folder)
		for pic in rawPics:
			pics.append(filedir + folder + "/" + pic)
	perform_pic_loop(pics)


def perform_sweep():
	return 0


# main function
def main():
	play_recent_files(con.PICS)
	while True:
		play_all_files(con.PICS)
		
# initialise screen
pygame.init()
screen = pygame.display.set_mode((con.SCREENWIDTH, con.SCREENHEIGHT), pygame.FULLSCREEN)
screen.fill((255, 255, 255))
pygame.display.flip()
		
# call to the main function
main()
