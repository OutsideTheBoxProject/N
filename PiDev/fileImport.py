import bluetooth
import sys, time
import constants as con

sock=bluetooth.BluetoothSocket( bluetooth.RFCOMM )


# tests a connection and finds out whether the headband is in reach
# for data import
# returns true or false according to connection
def test_connection():
	global sock
	sock = bluetooth.BluetoothSocket( bluetooth.RFCOMM )
	bd_addr = "00:06:66:74:81:23" #Headband address
	port = 1
	try:
		sock.connect((bd_addr, port))
		#print 'Connected'
		return True
	except bluetooth.BluetoothError as bte:
		print bte
		return False
		
		
# creates a folder name according to the time stamp
def get_folder_name():
	return  con.PICS + time.strftime("%Y-%m-%d-%H-%M-%S") + "/"

# does the actual file transfer
def get_files_from_headband():
	global sock
	
	sock.send('GO') 
	command = '' 
	buf = ''
	data = ''
	
	folder = get_folder_name()
	
	while command!='$HSEnd':
		buf = ''
		while buf.find('-end') == -1:
			data = ''
			data = sock.recv(1024)
			buf = buf + data
#		print "buf: [%s]" % buf
		if buf.find('$HSStart') != -1:
			command = '$HSStart'
		elif buf.find('$HSEnd') != -1:
			command = '$HSEnd'
		elif buf.find('$HSFile:') != -1:
			filename = buf[8:buf.find('|')]
			size = int(buf[buf.find('|')+1:buf.find('-end')])
			print filename, size,
			f = open(folder + filename, 'wb+')
			while size > 0:
				data = sock.recv(min(size,1024))
				size -= len(data)
				f.write(data)
#				print size
			#print "  Written to file"
			f.close()
	sock.close()
	#print 'Connection closed'
