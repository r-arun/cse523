import os
limit = 200
while True:
	test = os.system("./waiter_2d.o >> output_2d.txt")
	print "Returned ",test
	os.system("grep Trail output_2d.txt|wc -l > lines")
	fd = open("lines","rb")
	cnt = int(fd.read().strip())
	if(cnt >=limit):
		os.system("grep RATIO output_2d.txt|cut -d' '2|sort -r > out_2d_result_interval")
		os.system("grep RATIO output_2d.txt|cut -d' '3|sort -r > out_2d_result_movement")
		break
			
