# global imports
import PIL
from PIL import Image
import sys

# local imports
import constants as con


# main function
def resize_images(filedir):
	files = os.listdir()
	for f in files:
		img = Image.open(filedir + f)
		hpercent = (con.SCREENHEIGHT / float(img.size[1]))
		wsize = int((float(img.size[0]) * float(hpercent)))
		img = img.resize((wsize, con.SCREENHEIGHT), PIL.Image.ANTIALIAS)
		img.save()
		
	

# main call
resize_images(sys.argv[1])

