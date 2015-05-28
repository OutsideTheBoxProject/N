import bluetooth
import sys
bd_addr = "00:06:66:74:81:23" #Headband address

port = 1
sock=bluetooth.BluetoothSocket( bluetooth.RFCOMM )
sock.connect((bd_addr, port))
print 'Connected'

data = 'a'
while data!='':
	data = sock.recv(1)
	print '%s'%data

sock.close()
