# global imports
import os
import pygame
from pygame.locals import *
import PIL
from PIL import Image

# local imports
import constants as con

# global variables
running = 0

# implementation
def get_dir_content(filedir):
	contents = os.listdir(filedir)
	contents.sort()
	return contents


# actually show the pics
def perform_pic_loop(pics):
	global running, screen
	i = 0
	while True:
		# display pic
		if running == 0 or (pygame.time.get_ticks() - running) > con.WAITTIME:
			screen.fill((255, 255, 255))
			screen.blit(pygame.image.load(pics[i]), (0, 0))
			pygame.display.flip()
			running = pygame.time.get_ticks()
			i = i + 1
			if i == len(pics):
				i = 0
		
		for event in pygame.event.get():	
			#exit
			if event.type == pygame.KEYDOWN:
				if event.key == pygame.K_ESCAPE:
					print "Goodbye."
					exit()
	


# main function
def play_recent_files(filedir):
	recent = get_dir_content(filedir)[-1]
	rawPics = get_dir_content(filedir + recent)
	pics = []
	for pic in rawPics:
		pics.append(filedir + recent + "/" + pic)
	perform_pic_loop(pics)
	

# initialise screen
pygame.init()
screen = pygame.display.set_mode((con.SCREENWIDTH, con.SCREENHEIGHT), pygame.FULLSCREEN)
screen.fill((255, 255, 255))
pygame.display.flip()
		
# call to the main function
play_recent_files(con.PICS)
