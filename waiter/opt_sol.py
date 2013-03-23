import itertools
import random
from copy import copy

GMIN = 2**32
GMAX = -GMIN

def median(points):
	#Points is sorted
	"""Given a sorted list of points, finds the median.
	If lenght of the list is
		 even, returns a pair of two middle elements.(middle -1 , middle +1)
		 odd, return a pair (middle,middle)"""
	n = len(points)
	p1 = n/2
	if(n%2 == 0):
		p2 = p1 -1
	else:
		p2 = p1
	return points[p2],points[p1]

def perform_test(points):
	"""Given a set of points, generates all permutations and places the points 
	according to the permutation and calculates the values of cg"""
	#run tests on the permutation of points and print the result out
	assert(len(points))
	print points, sum(points)*1.0/len(points)
	perm = itertools.permutations(points)
	for arr in perm:
		print arr,stat(arr,1)

def stat(perm,detailed = 0):
	"""Given a permutation, find the range and total movement of cg"""
	cmin, cmax = GMIN, GMAX
	movement = 0.0
	total = 0.0
	old_cg = 0.0
	for ind in xrange(len(perm)):
		total = total + perm[ind]
		cg = total/(ind+1)
		if(cmin > cg): cmin = cg
		if(cmax < cg): cmax = cg
		if(ind > 0):
			#first cg does not imply movement
			movement += abs(cg - old_cg)
		old_cg = cg
	if(detailed):
		return (cmax, cmin, abs(cmax-cmin), movement)
	return (abs(cmax-cmin),movement)


def heuristic1(points):
	"""Place the next point such that the new ci moves close to cn"""
	points = copy(points)
	assert(len(points))
	cn = sum(points)*1.0/len(points)
	print "CN",cn
	total = 0.0
	count = 0
	perm = []
	while(len(points) > 0):
		count += 1
		best_ci = points[0]
		best_ci_pos = 0
		for i in xrange(len(points)):
			pi = points[i]
			ci = (total + pi)/count
			if(abs(best_ci - cn) > abs(ci -cn)):
				best_ci = ci
				best_ci_pos = i
		perm.append(points[best_ci_pos])
		total += points[best_ci_pos]
		print total
		print total/count
		del points[best_ci_pos]
	return perm

def find_opt(points, option = 0):
	"""Returns all optimal permutations
		option - 0 is to minimize the range
				 1 is to minimize the total sum- that is movement of cg"""
	#heuristic 1 - minimize the range
	#aim to test if median is part of all opt solutions
	opt_result = []
	perm = itertools.permutations(points)
	#an iterator of all permutations
	best_range = GMIN
	min_movement = 0
	for arr in perm:
		current_range, movement = stat(arr)
		if(option == 0):
			if(current_range < best_range):
				print "CHANGE ",current_range,arr
				best_range = current_range
				opt_result = [arr]
			elif(current_range == best_range):
				print "STATUSQUO ",current_range,arr
				opt_result.append(arr)
		elif(option == 1):
			if(min_movement < movement):
				opt_result = [arr]
			elif(min_movement == movement):
				opt_result.append(arr)
	return opt_result

if(__name__=='__main__'):
	print "OPT Result"
	test_case = [8,-1,0,1,3]
	print find_opt(test_case)
	print "HEURISTIC"
	print heuristic1(test_case)
print stat([1,3,0,-1,8],0)
print test_case
perform_test(test_case)