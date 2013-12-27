import random
import os
import sys

count = 0
while count < 200:
	count += 1
	num = str(random.randint(4, 3000))
	ret1 = os.system("echo "+num+"| ./test_convex_hull.o  > out_"+num)
	if(ret1!=0):
		print "First Failed"
		sys.exit(0)
	fd = open("out_"+num, 'rb')
	string = fd.read()
	fd.close()
	c_str = "Convex Hull\n"
	pos = string.find(c_str)
	end_pos = pos + len(c_str)
	result1 = string[end_pos:]
	od = open("in_"+num, 'wb')
	od.write(string[:pos])
	od.close()
	ret2 = os.system("./2dch.o < in_"+num +" > result")
	if(ret2!=0): print "Second Failed"
	fd = open("result", 'rb')
	result2 = fd.read()
	arr1 = result1.split()
	arr2 = result2.split()
	l1 = len(arr1)
	l2 = len(arr2)
	if(l1 != l2):
		print "Fail ",l1 ,l2
		continue
	ind = arr2.index(arr1[0])
	if(ind < 0):
		print "Fail Mismatch"
		continue
	for i in xrange(len(arr1)):
		if(arr1[i] != arr2[(ind+i)%l1]):
			print "Fail Not equal"
			print arr1
			print arr2
			continue
	print "Success"
