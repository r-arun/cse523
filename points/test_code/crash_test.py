import sys
import os

cnt = 100
while True:
	v = os.system("./a.out >out_fail <biginp")
	if(v > 0):
		print "Crashed!"
		break
	cnt = cnt-1
	if(cnt < 0): break
