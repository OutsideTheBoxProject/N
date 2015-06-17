import constants as con
import os, time

# this is just a collection of logging functions in order to have them
# not clogging the main file
# most of them are only used during evaluation anyway 

# append a single line at the end of a text file
def append_line(filename, line):
	with open(filename, "a") as f:
		f.write(line + "\n")

# get the contents of a file as an array of lines
def get_lines(filename):
	with open(filename) as f:
		return f.readlines()

# update the filename for a deleted picture in the move folder reference
def update_picture_filename(filename, pfilename):
	lines = get_lines(filename)
	os.remove(filename)
	for line in lines:
		if line.split(",")[1] == pfilename:
			append_line(con.MOVEFOLDER + filename, line.strip())
		else:
			append_line(filename, line.strip())
			
# get the correctly formatted timestamp for the local time	
def get_cur_timestamp():
	return time.strftime("%Y-%m-%d %H:%M:%S")

# returns the correctlz formatted timestamp for the day
def get_date_timestamp():
	return time.strftime("%Y-%m-%d")
	
# formats the info with timestamp and delimiter
def get_line(info, detail=""):
	line = []
	line.append(get_cur_timestamp())
	line.append(info)
	line.append(detail)
	return ",".join(line)

# logging station starting
def log_start_station():
	append_line(con.STATIONLOG, get_line("starting application"))
		
# logs the deletion of a picture
def log_picture_deletion(pfilename):
	if con.SWEEPMODE == con.DELETE:
		append_line(con.STATIONLOG, get_line("deleted picture", pfilename))
	elif con.SWEEPMODE == con.MOVE:
		append_line(con.STATIONLOG, get_line("archived picture", pfilename))	
	elif con.SWEEPMODE == con.PRINT:
		append_line(con.STATIONLOG, get_line("would delete picture", pfilename))
	else:
		append_line(con.STATIONLOG, get_line("error: procedure unspecified"))
	
# logs the initiation of data transfer
def log_data_transfer_start():
	append_line(con.STATIONLOG, get_line("initiated data transfer"))

# logs finishing the data transfer
def log_data_transfer_finish():
	append_line(con.STATIONLOG, get_line("data transfer finished"))

# logs new cycle of showing pictures
def log_picture_cycle():
	append_line(con.STATIONLOG, get_line("showing all pictures"))

# logs the case that there are no pictures to be found, if that is the case
def log_no_pictures():
    append_line(con.STATIONLOG, get_line("no pictures to be found"))

