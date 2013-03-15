import itertools
import random

def median(points):
	#Points is sorted
	n = len(points)
	p1 = n/2
	if(n%2 == 0):
		p2 = p1 -1
	else:
		p2 = p1
	return points[p2],points[p1]

def perform_test(points):
	#run tests on the permutation of points and print the result out
	perm = itertools.permutations(points)
	for arr in perm:
		_min, _max = 1024, -1024
		_cg = 0.0
		total = 0.0
		_cg_sum = 0.0
		for ind  in xrange(len(arr)):
			point = arr[ind]
			total += point
			_cg = total/(ind+1)
			if(_cg > _max): _max = _cg
			if(_cg < _min): _min = _cg
			_cg_sum += _cg
		print arr, _cg, _max, _min, _max-_min, _cg_sum

def find_opt(points):
	#heuristic 1 - minimize the range
	#aim to test if median is part of all opt solutions
	n = len(points)
	opt_result = []
	center = sum(points)/len(points)
	p1,p2 = median(points)
	perm = itertools.permutations(points)
	cg_min, cg_max = 2**32, -(2**32)
	for arr in perm:
		_min, _max = 2**32, -(2**32)
		tot,cg = 0, 0
		for j in xrange(len(arr)):
			tot += arr[j]
			cg = tot/(j+1)
			if(_min > cg):
				_min = cg
			if(_max < cg):
				_max = cg
		_range = abs(_max - _min)
		c_range = abs(cg_max - cg_min)
		if(_range < c_range):
			cg_max, cg_min = _max, _min
			opt_result = [arr]
		elif(_range == c_range):
			opt_result.append(arr)
	count = 0
	"""
	print "POINTS"
	print points
	print "OPT RESULT"
	print opt_result
	for arr in opt_result:
		if(arr[0] == p1 or arr[0] == p2):
			count+=1
	print "%d out of %d start with median" %(count, len(opt_result))
	"""
	return opt_result

test = 0
if(__name__=='__main__'):
	print "OPT Result"
	print find_opt
	perform_test([-3,-2,-1,0,1,2,3])

for i in xrange(test):
	_arr_size = 9
	arr = []
	for i in xrange(_arr_size):
		val = random.randint(1, 10000000)
		if val not in arr:
			arr.append(val)
	arr.sort()
	find_opt(arr)

		
