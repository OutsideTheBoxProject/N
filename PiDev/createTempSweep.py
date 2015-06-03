import os, PIL
import constants as con

def get_dir_content(filedir):
	contents = os.listdir(filedir)
	contents.sort()
	return contents
	
def write_sweep(folder, file_number, timestamp):	
	with open(con.SWEEPFILE, "a") as f:
		f.write(str(folder) + ", " + str(file_number) + ", " + timestamp + "\n" )
	
def create_sweep_data(filedir):
	folders = get_dir_content(filedir)
	for folder in folders:
		content = get_dir_content(filedir + folder)
		file_number = len(content)
		timestamp = content[0].split(" ")[0]
		write_sweep(filedir + folder + "/", file_number, timestamp)

create_sweep_data(con.PICS)
