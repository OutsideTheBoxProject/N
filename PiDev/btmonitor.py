import bluetooth
import sys
bd_addr = "00:06:66:74:81:23" #Headband address

port = 1
sock=bluetooth.BluetoothSocket( bluetooth.RFCOMM )
sock.connect((bd_addr, port))
print 'Connected'
sock.send('GO') 		

command = '' 
buf = ''
data = ''

while command!='$HSEnd':
	buf = ''
	while buf.find('-end') == -1:
		data = ''
		data = sock.recv(1024)
		buf = buf + data
#	print "buf: [%s]" % buf
	if buf.find('$HSStart') != -1:
		command = '$HSStart'
	if buf.find('$HSEnd') != -1:
		command = '$HSEnd'
	if buf.find('$HSFile:') != -1:
		filename = buf[8:buf.find('|')]
		size = int(buf[buf.find('|')+1:buf.find('-end')])
		print filename, size,
		f = open('import/' + filename, 'wb+')
		while size > 0:
			if size > 1024:
				data = sock.recv(1024)
			else:
				data = sock.recv(size)
			size -= len(data)
			f.write(data)
#			print size
		print "  Written to file"
		f.close()


print 'Connection closed'
sock.close()
