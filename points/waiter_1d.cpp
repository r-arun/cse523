#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <ctime>
#include <climits>
#include <cmath>
#define FUZZ 1

using namespace std;
typedef vector <double> VECD;
double get_interval(VECD input){
	double total = 0.0;
	double c_min = (double) INT_MAX, c_max = (double) INT_MIN;
	for(int i = 0; i < input.size(); ++i){
		total += input[i];
		double c_i = total/(i+1);
		if(c_i < c_min){
			c_min = c_i;
		}
		if(c_i > c_max){
			c_max = c_i;
		}
	}
	return c_max-c_min;
}
VECD arrange_points(VECD input){
	VECD best;
	double min_interval = (double) INT_MAX;
	do{
		double current_range = get_interval(input);	
		if(current_range < min_interval){
			min_interval = current_range;
			best = input;
		}
	}while(next_permutation(input.begin(), input.end()));
	return best;
}
void print(VECD input){
	for(VECD::iterator it = input.begin(); it != input.end(); ++it){
		cout << *it << " ";
	}
	cout << endl;
}
double center_of_mass(VECD input){
	if((int)input.size() == 0)
		return 0; 
	double total = 0.0;
	for(VECD::iterator it = input.begin(); it!= input.end(); ++it){
		total += *it;
	}
	return total/input.size();
}
VECD closest_to_cn_heuristic(VECD input){
	double c_n = center_of_mass(input);
	VECD perm;
	VECD copy = input;
	while((int) copy.size() > 0){
		double dist = (double) INT_MAX;
		VECD::iterator best_point;
		for(VECD::iterator it = copy.begin(); it != copy.end(); ++it){
			VECD new_perm = perm;
			new_perm.push_back(*it);
			double new_cg = center_of_mass(new_perm);
			cout << *it << " " << new_cg << "  " << c_n << endl;
			double diff = abs(new_cg - c_n);
			if(diff < dist){
				dist = diff;
				best_point = it;	
			}
		}
		perm.push_back(*best_point);
		copy.erase(best_point);
	}
	return perm;
}
VECD closest_to_prev_heuristic(VECD input){
	double c_prev = center_of_mass(input);
	VECD perm;
	VECD copy = input;
	double total = 0.0;
	while((int) copy.size() > 0){
		double dist = (double) INT_MAX;
		VECD::iterator best_point;
		for(VECD::iterator it = copy.begin(); it != copy.end(); ++it){
			VECD new_perm = perm;
			new_perm.push_back(*it);
			double new_cg = center_of_mass(new_perm);
			cout << *it << " " << new_cg << "  " << c_prev << endl;
			double diff = abs(new_cg - c_prev);
			if(diff < dist){
				dist = diff;
				best_point = it;	
			}
		}
		perm.push_back(*best_point);
		copy.erase(best_point);
		c_prev = center_of_mass(perm);
	}
	return perm;
}
vector < VECD >  generate_perm(VECD fixed, VECD unfixed){
	if((int) unfixed.size() == 0){
		vector < VECD > result;
		result.push_back(fixed);
		return result;
	}
	vector < VECD > result;
	for(int i = 0; i < unfixed.size(); ++i){
		VECD perm = fixed;
		perm.push_back(unfixed[i]);
		VECD remaining;
		for(int j = 0; j < unfixed.size(); ++j){
			if(i != j){
				remaining.push_back(unfixed[j]);
			}
		}
		vector < VECD > t = generate_perm(perm, remaining);
		for(vector <VECD>::iterator it = t.begin(); it != t.end(); ++it){
			result.push_back(*it);
		}
	}
	return result;
}
vector < VECD > permute(VECD input){
	vector <VECD> result;
	for(int i = 0; i < input.size(); ++i){
		VECD fixed;
		VECD unfixed;
		fixed.push_back(input[i]);
		for(int j = 0; j < input.size(); ++j){
			if(i != j){
				unfixed.push_back(input[j]);
			}
		}
		vector < VECD > t = generate_perm(fixed, unfixed);
		for(vector <VECD>::iterator it = t.begin(); it != t.end(); ++it){
			result.push_back(*it);
		}
	}
	return result;
}
VECD branch_bound_arrange_points(VECD fixed,VECD unfixed, VECD best, double best_interval,
							double total, double cur_min, double cur_max){
	//state needed. fixed, unfixed, best interval, total, current_min, current_max
	//aim is to find the optimal interval
	//pick last from fixed and compute the interval change.
	if((int)fixed.size() > 0){
		total += fixed[(int)fixed.size()-1];
		double cg = total / (int)fixed.size();
		if(cg < cur_min){
			cur_min = cg;
		}
		if(cg > cur_max){
			cur_max = cg;
		}
		if(cur_max-cur_min >= best_interval){
			return best;	
		}
		if((int)unfixed.size() == 0){
			return fixed;		
		}
	}
	for(int i = 0; i < (int)unfixed.size(); ++i){
		VECD u = fixed; 	
		VECD v;
		if((int)fixed.size() == 0){
			cur_min = (double) INT_MAX;
			cur_max = (double) INT_MIN;
			total = 0;
		}
		u.push_back(unfixed[i]);
		for(int j = 0; j < (int)unfixed.size(); ++j){
			if(i != j){
				v.push_back(unfixed[j]);
			}
		}
		VECD b = branch_bound_arrange_points(u, v, best, best_interval, total, cur_min, cur_max);
		if(b != best){
			best_interval = get_interval(b);
			best = b;
		}
	}
	return best;
}
bool is_equal(vector <double> a, vector <double> b){
	if(a.size() != b.size()) return false;
	for(int i = 0; i < a.size(); ++i){
		if(a[i] != b[i]) return false;
	}
	return true;
}

VECD random_points(int size){
	VECD points;
	while(size--){
		points.push_back(rand()%10000);
	}
	return points;
}

int main(){
	/*
	double a [] = {1,  3, 4 , 5 , 8, 12, 9};
	VECD arr = VECD (a, a+sizeof(a)/sizeof(a[0]));
	int ind = 0;
	vector <VECD> perm =  permute(arr);
	do{
		assert(is_equal(perm[ind], arr));

		++ind;
	}while(next_permutation(arr.begin(), arr.end()));
	*/
	srandom(time(NULL));
	int size;
	cin >> size;
	int fuzz_trails = 1000;
	do{
	cout << "Fuzz Trail " <<fuzz_trails << endl;
	VECD arr = random_points(size);
	VECD heuristic0 = closest_to_cn_heuristic(arr);
	print(heuristic0);

	/*
	sort(arr.begin(),arr.end());
	VECD original = arrange_points(arr);
	print(original);
	*/
	VECD fixed, unfixed, best;
	unfixed = arr;
	best = branch_bound_arrange_points(fixed, unfixed, arr, (double) INT_MAX, 0.0, (double) INT_MAX, (double) INT_MAX);
	print(best);
	/*
	assert(original.size() == best.size());
	for(int i = 0; i < original.size(); ++i){
		assert(original[i] == best[i]);
	}*/
	double h0 = get_interval(heuristic0);
	double g0 = get_interval(best);
	cout << h0 << " " <<g0 << endl;
	cout << center_of_mass(best) << endl;
	cout << center_of_mass(heuristic0) << endl;
	double ratio = h0/g0;
	cout << ratio << endl;
	assert(ratio <=2.0);
	}while(FUZZ && fuzz_trails--);
	return 0;
}
