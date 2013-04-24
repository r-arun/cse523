"""Given a list, generate all possible permutations of taking 2 points at a time"""
import copy
def two_points(arr):
	if(len(arr) <= 1):
		return [arr]
	else:
		result = []
		for i in xrange(len(arr)):
			barr = copy.copy(arr)
			x = barr[i]
			del barr[i]
			for j in xrange(len(barr)):
				carr = copy.copy(barr)
				y = carr[j]
				del carr[j]
				suffix_arr = two_points(carr)
				for res in suffix_arr:
					current =[x,y]
					current.extend(res)
					result.append(current)
	return result

def one_point(arr):
	if(len(arr) <= 1):
		return [arr]
	result = []
	for i in xrange(len(arr)):
		barr = copy.copy(arr)
		x = barr[i]
		print barr,x
		del barr[i]
		lower = one_point(barr)
		for res in lower:
			res.append(x)
			result.append(res)
	return result

arr = one_point([1, 2, 3 ,4])
print len(arr)
for i in arr:
	print i
arr = two_points([1, 2, 3 ,4])
print len(arr)
for i in arr:
	print i



