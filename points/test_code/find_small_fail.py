import sys
import os

cnt = 100
while True:
	v = os.system("./a.out >out10 <smallinp")
	if(v > 0):
		print "Success!"
		break
	os.system("./a.out <biginp > /dev/null 2> /dev/null")
	cnt =cnt-1
	if(cnt < 0):
		break

