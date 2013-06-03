import sys
import itertools
import random
from copy import copy

GMIN = 2**32
GMAX = -GMIN

def generate_test(num):
	arr = []
	while len(arr) < num:
		inp = random.random()
		e = random.randint(0,10)
		if(e > 5):
			inp = inp * -1
		arr.append(inp)
	return arr

def median(points):
	#Points is sorted
	"""Given a sorted list of points, finds the median.
	If length of the list is
	even,
		returns a pair of two middle elements.(middle -1 , middle +1)
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

def stat2(perm, detailed = 0):
	"""Find the range and total movement of cg, placing two points simulataneoulsy at a time"""
	i = 0
	tot = 0
	cnt = 0
	cmin, cmax = GMIN, GMAX
	old_cg = 0.0
	movement = 0.0
	while i < len(perm):
		cnt += 1
		tot += perm[i]
		i += 1
		if(i < len(perm)):
			cnt += 1
			tot += perm[i]
			i += 1
		cg = tot/ (cnt*1.0)
		if(cmin > cg): cmin = cg
		if(cmax < cg): cmax = cg
		if(cnt > 2): movement += (abs (cg - old_cg))
		old_cg = cg
	if(detailed):
		return (cmax, cmin, abs(cmax-cmin), movement)
	return (abs(cmax-cmin),movement)
			
	
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

def average(i,j):
	return (i+j)*1.0 /2.0

def getBalancingPair(points, cn , tot_p, cnt, delta = -1):
	assert(len(points))
	if(len(points) == 1):
		return (points[0],)
	min_pair = None
	#ci = (tot_p + min_pair[0] + min_pair[1])*1.0/ (cnt+2.0)
	min_cdiff = GMIN
	if(delta >=  0):
		for i_ind in xrange(len(points)):
			for j_ind in xrange(len(points)):
				if(i_ind == j_ind): continue
				i = points[i_ind]
				j = points[j_ind]
				if(abs(i - j) > delta): continue
				ci = (tot_p + i + j)*1.0/(cnt+2.0)
				cdiff = abs(cn - ci)
				if(min_cdiff > cdiff):
					min_cdiff = cdiff
					min_pair = (i,j)
	if(min_pair):
		return min_pair
	for i_ind in xrange(len(points)):
		for j_ind in xrange(len(points)):
			if(i_ind == j_ind): continue
			i = points[i_ind]
			j = points[j_ind]
			ci = (tot_p + i + j)*1.0/(cnt+2.0)
			cdiff = abs(cn - ci)
			if(min_cdiff > cdiff):
				min_cdiff = cdiff
				min_pair = (i,j)
	return min_pair


def getClosestPair(points, cg):
	"""From the given set of points, pick 2 points such that
	new c_i is closest to cn"""
	assert(len(points))
	if(len(points) == 1):
		return (points[0],)
	min_pair = (points[0], points[1])
	sel_average = average(points[0] , points[1])
	min_diff = abs(sel_average - cg)
	for i_ind in xrange(len(points)):
		for j_ind in xrange(len(points)):
			if(i_ind == j_ind): continue
			i = points[i_ind]
			j = points[j_ind]
			cur_diff = abs(average(i, j) - cg)
			if(min_diff > cur_diff):
				min_pair = (i,j)
				min_diff = cur_diff
	return min_pair

def heuristic4(point, delta):
	points = copy(point)
	assert(len(points))
	points.sort()
	perm = []
	cn = sum(points) * 1.0 / (len(points) * 1.0)
	tot = 0
	cnt = 0
	while(len(points) > 0):
		ret = getBalancingPair(points, cn, tot, cnt, delta)
		if(len(ret) == 2):
			perm.append(ret[0])
			perm.append(ret[1])
			tot += (ret[0] + ret[1])
			cnt += 2
			points.remove(ret[0])
			points.remove(ret[1])
		else:
			perm.append(ret[0])
			tot += (ret[0]) #unnecessary
			cnt += 1
			points.remove(ret[0])
	return perm


def heuristic3(point):
	"""Pick two points such that ci remains closest to cn.
	"""
	points = copy(point)
	assert(len(points))
	stat(points)
	points.sort()
	perm = []
	cn = sum(points) * 1.0 / (len(points) * 1.0)
	tot = 0
	cnt = 0
	while(len(points) > 0):
		ret = getBalancingPair(points, cn, tot, cnt)
		if(len(ret) == 2):
			perm.append(ret[0])
			perm.append(ret[1])
			tot += (ret[0] + ret[1])
			cnt += 2
			points.remove(ret[0])
			points.remove(ret[1])
		else:
			perm.append(ret[0])
			tot += (ret[0]) #unnecessary
			cnt += 1
			points.remove(ret[0])
	return perm

def heuristic2(point):
	"""Select two points such that their average is closest to cn
		This may be different from heuristic1 where ci always moves
		closer to cn. Here the average is closest to cn but ci may not
		move closer to cn. If there is only one point left, it is put in
		place."""
	points = copy(point)
	assert(len(points))
	stat(points)
	points.sort()
	perm = []
	cn = sum(points)*1.0/(len(points)*1.0)
	while(len(points) > 0):
		ret = getClosestPair(points, cn)
		if(len(ret) == 2):
			perm.append(ret[0])
			perm.append(ret[1])
			points.remove(ret[0])
			points.remove(ret[1])
		else:
			perm.append(ret[0])
			points.remove(ret[0])
	return perm

def heuristic6(point):
	"""Select two points such that the new ci is closest to ci-1
	This aims to greedily limit the distance ci moves apart from ci-1."""
	points = copy(point)
	assert(len(points))
	stat(points)
	points.sort()
	perm = []
	cprev = sum(points)*1.0/(len(points)*1.0)
	while(len(points) > 0):
		ret = getBalancingPair(points, cprev, sum(perm), len(perm))
		if(len(ret) == 2):
			perm.append(ret[0])
			perm.append(ret[1])
			points.remove(ret[0])
			points.remove(ret[1])
		else:
			perm.append(ret[0])
			points.remove(ret[0])
		cprev = sum(perm)/len(perm)
	return perm


def heuristic0(points, delta = -1):
	"""Place the next point such that the new ci remains close to ci+1. A positive delta first forces the search within the (-delta, delta) interval"""

	points = copy(points)
	assert(len(points))
	cn = sum(points)*1.0/len(points)
	prev_ci = cn
	#print "CN",cn
	total = 0.0
	count = 0
	perm = []
	while(len(points) > 0):
		count += 1
		best_ci = GMIN
		best_ci_pos = -1

		if(delta >= 0):
			for i in xrange(len(points)):
				pi = points[i]
				if(not (cn - delta < pi < cn + delta)):
					continue
				ci = (total + pi) / count
				if(abs(best_ci - prev_ci) > abs(ci -prev_ci)):
					best_ci = ci
					best_ci_pos = i

		if(best_ci_pos < 0):
			for i in xrange(len(points)):
				pi = points[i]
				ci = (total + pi) / count
				if(abs(best_ci - prev_ci) > abs(ci -prev_ci)):
					best_ci = ci
					best_ci_pos = i

		assert(best_ci_pos >= 0)
		prev_ci = best_ci
		perm.append(points[best_ci_pos])
		total += points[best_ci_pos]
		#print total
		#print total/count
		del points[best_ci_pos]
	return perm

def heuristic1(points):
	"""Place the next point such that the new ci remains close to cn"""
	points = copy(points)
	assert(len(points))
	cn = sum(points)*1.0/len(points)
	#print "CN",cn
	total = 0.0
	count = 0
	perm = []
	while(len(points) > 0):
		count += 1
		best_ci = (total + points[0]) / count
		best_ci_pos = 0
		for i in xrange(len(points)):
			pi = points[i]
			ci = (total + pi)/count
			if(abs(best_ci - cn) > abs(ci -cn)):
				best_ci = ci
				best_ci_pos = i
		perm.append(points[best_ci_pos])
		total += points[best_ci_pos]
		#print total
		#print total/count
		del points[best_ci_pos]
	return perm

#garr = []
def find_opt(points, option = 0):
	"""Returns all optimal permutations
		option - 0 is to minimize the range
				 1 is to minimize the total sum- that is movement of cg"""
	#heuristic 1 - minimize the range
	#aim to test if median is part of all opt solutions
	global perm_arr
	opt_result = []
	perm = itertools.permutations(points)
	#an iterator of all permutations
	best_range = GMIN
	min_movement = GMIN
	for arr in perm:
		#garr.append(list(arr))
		current_range, movement = stat(arr)
		if(option == 0):
			if(current_range < best_range):
				best_range = current_range
				opt_result = [arr]
			elif(current_range == best_range):
				opt_result.append(arr)
		elif(option == 1):
			if(movement < min_movement):
				opt_result = [arr]
				min_movement = movement
			elif(min_movement == movement):
				opt_result.append(arr)
	#perm_arr = garr
	return opt_result

def find_opt2(points, option = 0):
	"""Returns all optimal permutations
		option - 0 is to minimize the range
				 1 is to minimize the total sum- that is movement of cg"""
	#heuristic 1 - minimize the range
	#aim to test if median is part of all opt solutions
	#global perm_arr
	opt_result = []
	perm = itertools.permutations(points)
	#an iterator of all permutations
	best_range = GMIN
	min_movement = GMIN
	for arr in perm:
		#garr.append(list(arr))
		current_range, movement = stat2(arr)
		if(option == 0):
			if(current_range < best_range):
				best_range = current_range
				opt_result = [arr]
			elif(current_range == best_range):
				opt_result.append(arr)
		elif(option == 1):
			if(movement < min_movement):
				opt_result = [arr]
				min_movement = movement
			elif(min_movement == movement):
				opt_result.append(arr)
	#perm_arr = garr
	return opt_result

if(__name__=='__main__'):
	cnt = 0
	perm_arr = []
	val = int(sys.argv[1])
	num_cases = int(sys.argv[2])
	fd0 = open("result/result0.txt",'wb')
	fd1 = open("result/result1.txt",'wb')
	fd2 = open("result/result2.txt",'wb')
	fd3 = open("result/result3.txt",'wb')

	if(val < 4):
		fd0.write("Ratio of Ranges of Center of mass\n")
		fd0.write("H0, H1, H2, H3, H4, H5, H6\n")

	if(val < 4):
		fd1.write("Ratio of Distance moved by Center of mass\n")
		fd1.write("H0, H1, H2, H3, H4, H5, H6\n")

	if(val < 4):
		fd2.write("Ratio of Ranges of Center of Mass: Single Pick vs Simultaneous Picks\n")
		fd2.write("H2 Single, H2-2 picks, H3 Single, H3-2 picks,H4 Single,H4-2 picks, H6 Single, H6- picks\n")

	if(val < 4):
		fd3.write("Ratio of Distance Moved by Center of Mass: Single Pick vs Simultaneous Picks\n")
		fd3.write("H2 Single, H2-2 picks, H3 Single, H3-2 picks,H4 Single,H4-2 picks, H6 Single, H6- picks\n")
	else:
		while cnt < 10:
			test_case = generate_test(num_cases)
			h0= heuristic0(test_case)
			h1= heuristic1(test_case)
			h2= heuristic2(test_case)
			h3= heuristic3(test_case)
			h6= heuristic6(test_case)

			h0_stat = stat(h0,1)
			h1_stat = stat(h1,1)
			h2_stat = stat(h2,1)
			h3_stat = stat(h3,1)

			h4= heuristic4(test_case, min(h0_stat[2],h1_stat[1],h2_stat[2],h2_stat[2]))
			h5= heuristic0(test_case, min(h0_stat[2],h1_stat[1],h2_stat[2],h2_stat[2]))

			h4_stat = stat(h4,1)
			h5_stat = stat(h5,1)
			h6_stat = stat(h6,1)

			h2_stat2 = stat2(h2,1)
			h3_stat2 = stat2(h3,1)
			h4_stat2 = stat2(h4,1)
			h6_stat2 = stat2(h6,1)
			range1 = min(h0_stat[2],h1_stat[2],\
				h2_stat[2], h3_stat[2],\
				h4_stat[2], h5_stat[2], h6_stat[2])
			range2 = min(h2_stat2[2],h3_stat2[2],\
				h4_stat2[2], h6_stat2[2])
			dist1 = min(h0_stat[3],h1_stat[3],\
				h2_stat[3], h3_stat[3],\
				h4_stat[3], h5_stat[3], h6_stat[3])
			dist2 = min(h2_stat2[3],h3_stat2[3],\
				h4_stat2[3], h6_stat2[3])
			#Range stat1
			fd0.write("%f, %f, %f, %f, %f, %f, %f\n" %(h0_stat[2]/range1 , h1_stat[2]/range1 ,  h2_stat[2]/range1 ,  h3_stat[2]/range1 ,  h4_stat[2]/range1 ,  h5_stat[2]/range1 ,  h6_stat[2]/range1))

			#Distance stat1
			fd1.write("%f, %f, %f, %f, %f, %f, %f\n" %(h0_stat[3]/dist1, h1_stat[3]/dist1, h2_stat[3]/dist1, h3_stat[3]/dist1, h4_stat[3]/dist1, h5_stat[3]/dist1, h6_stat[3]/dist1))

			#print "PICK 2 H6", h6_stat2
			fd2.write("%f, %f, %f, %f, %f, %f, %f, %f\n" %(h2_stat[2]/range1,h2_stat2[2]/range2,\
			h3_stat[2]/range1, h3_stat2[2]/range2,\
			h4_stat[2]/range1,h4_stat2[2]/range2,\
			h6_stat[2]/range1,h6_stat2[2]/range2))

			#Distance stat2
			fd3.write("%f, %f, %f, %f, %f, %f, %f, %f\n" %(h2_stat[3]/dist1,h2_stat2[3]/dist2,\
			h3_stat[3]/dist1, h3_stat2[3]/dist2,\
			h4_stat[3]/dist1,h4_stat2[3]/dist2,\
			h6_stat[3]/dist1,h6_stat2[3]/dist2))
			cnt += 1

	while cnt < 10:
		#test_case = generate_test(random.randint(1,10))
		test_case = generate_test(num_cases)
		#test_case = [440, 295, 466, 596, 836, -905, 981] 
		#print "Test Case:",test_case
		#print "Optimal Solution:",
		opt_range = find_opt(test_case)
		opt_dist = find_opt(test_case, 1)
		opt_range2 = find_opt2(test_case)
		opt_dist2 = find_opt2(test_case,1)
		#print "perm_arr", len(perm_arr)
		#print "Average ",sum(test_case)*1.0/len(test_case)*1.0
		#print "Optimal Statistics"
		opt_stat_range = stat(list(opt_range[0]),1)
		opt_stat_dist = stat(list(opt_dist[0]),1)
		opt_stat_range2 = stat2(list(opt_range2[0]),1)
		opt_stat_dist2 = stat2(list(opt_dist2[0]),1)
		range1 = opt_stat_range[2]
		range2 = opt_stat_range2[2]
		dist1 = opt_stat_dist[3]
		dist2 = opt_stat_dist2[3]
		#print opt_stat_range
		#print opt_stat_dist
		#print "Heuristics"
		h0= heuristic0(test_case)
		h1= heuristic1(test_case)
		h2= heuristic2(test_case)
		h3= heuristic3(test_case)
		h4= heuristic4(test_case, range1)
		h5= heuristic0(test_case, range1)
		h6= heuristic6(test_case)
		#h0_stat2 = stat2(h0,1)
		#h1_stat2 = stat2(h1,1)
		h2_stat2 = stat2(h2,1)
		h3_stat2 = stat2(h3,1)
		h4_stat2 = stat2(h4,1)
		#h5_stat2 = stat2(h5,1)
		h6_stat2 = stat2(h6,1)

		h0_stat = stat(h0,1)
		h1_stat = stat(h1,1)
		h2_stat = stat(h2,1)
		h3_stat = stat(h3,1)
		h4_stat = stat(h4,1)
		h5_stat = stat(h5,1)
		h6_stat = stat(h6,1)

		#h5 is same as h0 with the change that there is a delta for it. h0 tries to keep c_i closest to c_{i-1}
		"""
		if(val == 5):
			print h0, perm_arr.index(h0)
			print h1, perm_arr.index(h1)
			print h2, perm_arr.index(h2)
			print h3, perm_arr.index(h3)
			print h4, perm_arr.index(h4)
			print h5, perm_arr.index(h5)
		"""

		#Range stat1
		fd0.write( "%f, %f, %f, %f, %f, %f, %f\n" %(h0_stat[2]/range1 , h1_stat[2]/range1 ,  h2_stat[2]/range1 ,  h3_stat[2]/range1 ,  h4_stat[2]/range1 ,  h5_stat[2]/range1 ,  h6_stat[2]/range1))

		#Distance stat1
		fd1.write("%f, %f, %f, %f, %f, %f, %f\n" %(h0_stat[3]/dist1, h1_stat[3]/dist1, h2_stat[3]/dist1, h3_stat[3]/dist1, h4_stat[3]/dist1, h5_stat[3]/dist1, h6_stat[3]/dist1))

		#Range Stat2
		print "OPT RANGE2", opt_stat_range2
		print "OPT DIST2", opt_stat_dist2
		#print "PICK 2 H2", h2_stat2
		#print "PICK 2 H3", h3_stat2
		print "PICK  H4", h4_stat
		print "PICK 2 H4", h4_stat2
		#print "PICK 2 H6", h6_stat2
		fd2.write("%f, %f, %f, %f, %f, %f, %f, %f\n" %(h2_stat[2]/range1,h2_stat2[2]/range2,\
		h3_stat[2]/range1, h3_stat2[2]/range2,\
		h4_stat[2]/range1,h4_stat2[2]/range2,\
		h6_stat[2]/range1,h6_stat2[2]/range2))

		#Distance stat2
		fd3.write("%f, %f, %f, %f, %f, %f, %f, %f\n" %(h2_stat[3]/dist1,h2_stat2[3]/dist2,\
		h3_stat[3]/dist1, h3_stat2[3]/dist2,\
		h4_stat[3]/dist1,h4_stat2[3]/dist2,\
		h6_stat[3]/dist1,h6_stat2[3]/dist2))
		if(val == 4):
			print "CMAX, CMIN, INTERVAL, MOVEMENT"
			print "H2", stat2(h2,1)
			print "H3", stat2(h3,1)
			print "H4", stat2(h4,1)
			print "H6", stat2(h6,1)
		cnt += 1
	fd0.close()
	fd1.close()
	fd2.close()
	fd3.close()
