# global imports
import PIL
from PIL import Image
import sys
import os

# local imports
import constants as con


# main function
def resize_images(filedir):
	files = os.listdir(filedir)
	for f in files:
		print "dealing with " + filedir + f
		img = Image.open(filedir + f)
		# hpercent = (600 / float(img.size[1]))
		# wsize = int((float(img.size[0]) * float(hpercent)))
		img = img.resize((con.PICTUREWIDTH, con.PICTUREHEIGHT), PIL.Image.ANTIALIAS)
		img.save(filedir + f)
	print "done."
		
	

# main call
resize_images(sys.argv[1])

