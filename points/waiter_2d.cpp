#include "convex_hull.h" 
#include <climits>
#include <ctime>
#include <fstream>
#define DEBUG_PRINT 0
#define FUZZ 1
#define RANDOM 1
#define DEBUG 0

vector <point *> arrange_cg(vector <point *> input){
	vector <point *> output;
	float x, y;
	x = y = 0;
	int cnt = 0;
	for(vector <point *> :: iterator it = input.begin(); it!= input.end();++it){
		x += (*it)->x;		
		y += (*it)->y;		
		++cnt;
		output.push_back(new point(x/cnt, y/cnt));
	}
	return output;
}
vector <point *> get_random_points(int size){
	vector <point *> input;
	while(size--){
		input.push_back(new point());
	}
	return input;
}
vector <point *> closest_to_cn_heuristic(vector <point *> input){
	vector <point *> perm;
	vector <point *> copy = input;
	point * c_n = center_of_mass(input);
	while((int) copy.size() > 0){
		double dist= (double)INT_MAX;
		vector<point *>::iterator best_point;
		for(vector<point *>::iterator it=copy.begin(); it!=copy.end(); ++it){
			vector <point *> new_perm = perm;
			new_perm.push_back(*it);	
			point * new_cg = center_of_mass(new_perm);
			double new_dist= new_cg->distance(c_n);
			if(new_dist < dist){
					dist = new_dist;
					best_point = it;
			}
		}	
		perm.push_back(*best_point);
		copy.erase(best_point);
	}
	return perm;
}
vector <point *> closest_to_prev_heuristic(vector <point *> input){
	vector <point *> perm;
	vector <point *> copy = input;
	point * c_prev = center_of_mass(input);
	while((int)copy.size() > 0){
		double dist= (double)INT_MAX;
		vector<point *>::iterator best_point;
		for(vector<point *>::iterator it=copy.begin(); it!=copy.end(); ++it){
			vector <point *> new_perm = perm;
			new_perm.push_back(*it);	
			point * new_cg = center_of_mass(new_perm);
			double new_dist= new_cg->distance(c_prev);
			if(new_dist < dist){
					dist = new_dist;
					best_point = it;
			}
		}	
		perm.push_back(*best_point);
		copy.erase(best_point);
		c_prev = center_of_mass(perm);	
	}
	return perm;
}
vector <point *> avg_closest_to_cn_heuristic_pair(vector <point *> input){
	vector <point *> perm;
	vector <point *> copy = input;
	point * c_n = center_of_mass(input);
	while((int) copy.size() > 1){
		double dist = (double)INT_MAX;
		pair <vector <point *> :: iterator, vector <point *> :: iterator> best_pair;
		for(vector <point *>::iterator p1 = copy.begin(); p1 != copy.end(); ++p1){
			for(vector <point *>:: iterator p2 = p1+1; p2 != copy.end(); ++p2){
				point * mid = new point(((*p1)->x+(*p2)->x)/2, ((*p1)->y+(*p2)->y)/2);
				double new_dist= mid->distance(c_n);
				if(new_dist < dist){
					dist = new_dist;
					best_pair = make_pair(p1, p2);
				}
			}
		}
		perm.push_back(*(best_pair.first));
		perm.push_back(*(best_pair.second));
		copy.erase(best_pair.second);
		copy.erase(best_pair.first);
	}
	if((int)copy.size() == 1){
		perm.push_back(copy[0]);
		copy.erase(copy.begin());
	}
	return perm;
}
vector <point *> closest_to_cn_heuristic_pair(vector <point *> input){
	vector <point *> perm;
	vector <point *> copy = input;
	point * c_n = center_of_mass(input);
	while((int) copy.size() > 1){
		double dist = (double)INT_MAX;
		pair <vector <point *> :: iterator, vector <point *> :: iterator> best_pair;
		for(vector <point *>::iterator p1 = copy.begin(); p1 != copy.end(); ++p1){
			for(vector <point *>:: iterator p2 = p1+1; p2 != copy.end(); ++p2){
				vector <point *> new_perm = perm;
				new_perm.push_back(*p1);
				new_perm.push_back(*p2);
				point * new_cg = center_of_mass(new_perm);
				double new_dist= new_cg->distance(c_n);
				if(new_dist < dist){
					dist = new_dist;
					best_pair = make_pair(p1, p2);
				}
			}
		}
		perm.push_back(*(best_pair.first));
		perm.push_back(*(best_pair.second));
		copy.erase(best_pair.second);
		copy.erase(best_pair.first);
	}
	if((int)copy.size() == 1){
		perm.push_back(copy[0]);
		copy.erase(copy.begin());
	}
	return perm;
}
vector <point *> closest_to_prev_heuristic_pair(vector <point *> input){
	vector <point *> perm;
	vector <point *> copy = input;
	point * c_prev = center_of_mass(input);
	while((int) copy.size() > 1){
		double dist = (double)INT_MAX;
		pair <vector <point *> :: iterator, vector <point *> :: iterator> best_pair;
		for(vector <point *>::iterator p1 = copy.begin(); p1 != copy.end(); ++p1){
			for(vector <point *>:: iterator p2 = p1+1; p2 != copy.end(); ++p2){
				vector <point *> new_perm = perm;
				new_perm.push_back(*p1);
				new_perm.push_back(*p2);
				point * new_cg = center_of_mass(new_perm);
				double new_dist= new_cg->distance(c_prev);
				if(new_dist < dist){
					dist = new_dist;
					best_pair = make_pair(p1, p2);
				}
			}
		}
		perm.push_back(*(best_pair.first));
		perm.push_back(*(best_pair.second));
		copy.erase(best_pair.second);
		copy.erase(best_pair.first);
		c_prev =  center_of_mass(perm);
	}
	if((int)copy.size() == 1){
		perm.push_back(copy[0]);
		copy.erase(copy.begin());
	}
	return perm;
}
double compute_displacement(vector <point *> input){
	double cur_dist = 0.0;
	//For every adjacent pair of numbers compute the distance and
	//add them up.
	for(int i = 0; i < input.size()-1; ++i){
		cur_dist += input[i]->distance(input[i+1]);
	}
	return cur_dist;
}

vector <point *> opt_displacement(vector <point *> input){
	sort(input.begin(), input.end());
	vector <point *> best;
	double dist = (double)INT_MAX;
	do{
		vector <point *> p = arrange_cg(input);
		double cur_dist = compute_displacement(p);
		if(cur_dist < dist){
			best = input;
			dist = cur_dist;
		}
		for(int i = 0; i < p.size(); ++i)
			delete(p[i]);
	}while(next_permutation(input.begin(), input.end()));
	return best;
}

vector <point *> opt_interval(vector <point *> input){
	sort(input.begin(), input.end());
	vector <point *> best;
	double area = (double)INT_MAX;
	do{
		vector <point *> p = arrange_cg(input);
		vector <point *> con_hull = convex_hull(p);
	#if DEBUG_PRINT
		cout << "Convex Hull" << endl;
		print(con_hull);
	#endif
		double a = area_of_polygon(con_hull);
		if(a < area){
			best = input;
			area = a;
		}
	}while(next_permutation(input.begin(), input.end()));
	return best;
}
vector <point *> read_points_from_file(string name){
	ifstream inp;
	vector <point *> v;
	inp.open(name.c_str());
	int cnt;
	inp >> cnt;
	while(cnt--){
		float a, b;
		inp >> a >> b;
		v.push_back(new point(a, b));
	}
	preprocess_vector(v);
#if DEBUG
	cout << "After preprocessing" <<endl;
	print(v);
#endif
	return  find_convex_hull(v);
}
vector <point *> arrange_cg_two_selection(vector <point *> input){
	vector <point *> output;
	float x, y;
	x = y = 0;
	int cnt = 0;
	for(vector <point *> :: iterator it = input.begin(); it!= input.end();++it){
		x += (*it)->x;		
		y += (*it)->y;		
		++cnt;
		if((cnt%2 == 0) || (cnt == input.size())){
			output.push_back(new point(x/cnt, y/cnt));
		}
	}
	return output;
}

int main(){
	int size;
	double i_max = 0, d_max = 0;
#if RANDOM
	cin>>size;
	srandom(time(NULL));
	int fuzz_count = 100;
	do{
	/*point * arr[] = {new point(0,10), new point(0,0), new point(10, 10),
		new point(10, 0), new point(7,7), new point(5,5), new point(5,7),
		new point(7,5)};
	vector <point *> input = vector <point *>(arr, arr+sizeof(arr)/sizeof(arr[0]));*/
	vector <point *> input = get_random_points(size);
#else
	string fname;
	cout << "Enter fname";
	cin>>fname;
	vector <point *> input = read_points_from_file(fname);
#endif
#if DEBUG
	cout << "Input " <<endl;
	print(input);
#endif
	vector <point *> best_interval = opt_interval(input);
	//OPT Interval
	double opt_interval_area = area_of_polygon(convex_hull(arrange_cg(best_interval)));
	double opt_interval_dist = compute_displacement(arrange_cg(best_interval));
#if DEBUG
	print(best_interval);
	cout << "Opt Interval Area " <<opt_interval_area <<endl;
#endif

	//OPT Displacement
	vector <point *> best_distance = opt_displacement(input);
	double opt_movement_area = area_of_polygon(convex_hull(arrange_cg(best_distance)));
	double opt_movement_dist = compute_displacement(arrange_cg(best_distance));
#if DEBUG
	print(best_distance);
	cout << "Opt Movement Displacement " <<opt_movement_dist <<endl;
#endif

	//Closest to c_n. Best heurisitic for interval
	vector <point *> res_cn = closest_to_cn_heuristic(input);
	vector <point *> h_mid_points_cn = arrange_cg(res_cn);
	vector <point *> h_hull_cn = convex_hull(h_mid_points_cn);
	double h_area_cn = area_of_polygon(h_hull_cn);
	double h_dist_cn = compute_displacement(h_mid_points_cn);
	double r_interval_0 = h_area_cn/opt_interval_area;
	double r_dist_0 = h_dist_cn/opt_movement_dist;
#if DEBUG
	print(res_cn);
	cout << "Heuristic Closest to cn Area "<< h_area_cn << " ";
	cout << h_area_cn/opt_interval_area << " ";
	cout << "Heuristic Closest to cn Displacement "<< h_dist_cn << " ";
	cout << h_dist_cn/opt_movement_dist << endl;
#endif

	//Closest to c_i. Best heurisitic for displacement
	vector <point *> res = closest_to_prev_heuristic(input);
	vector <point *> h_mid_points= arrange_cg(res);
	vector <point *> h_hull = convex_hull(h_mid_points);
	double h_area = area_of_polygon(h_hull);
	double h_dist = compute_displacement(h_mid_points);
	double r_interval_1 = h_area/opt_interval_area;
	double r_dist_1 = h_dist/opt_movement_dist;
#if DEBUG
	print(res);
	cout << "Heuristic Closest to Prev Area "<< h_area << " ";
	cout << h_area/opt_interval_area << " ";
	cout << "Heuristic Closest to Prev Displacement "<< h_dist<< " ";
	cout << h_dist/opt_movement_dist << endl;
#endif

	//Closest to c_n pair.
	vector <point *> res_cn_pair = closest_to_cn_heuristic_pair(input);
	vector <point *> h_mid_points_cn_pair = arrange_cg(res_cn_pair);
	vector <point *> h_hull_cn_pair = convex_hull(h_mid_points_cn_pair);
	double h_area_cn_pair = area_of_polygon(h_hull_cn_pair);
	double h_dist_cn_pair = compute_displacement(h_mid_points_cn_pair);
	double r_interval_2 = h_area_cn_pair/opt_interval_area;
	double r_dist_2 = h_dist_cn_pair/opt_movement_dist;

	vector <point *> h_mid_points_cn_pair_2 = arrange_cg_two_selection(res_cn_pair);
	vector <point *> h_hull_cn_pair_2 = convex_hull(h_mid_points_cn_pair_2);
	double h_area_cn_pair_2 = area_of_polygon(h_hull_cn_pair_2);
	double h_dist_cn_pair_2 = compute_displacement(h_mid_points_cn_pair_2);
	double r_interval_5 = h_area_cn_pair_2/opt_interval_area;
	double r_dist_5 = h_dist_cn_pair_2/opt_movement_dist;
#if DEBUG
	print(res_cn_pair);
	cout << "Heuristic CN Pair Area "<< h_area_cn_pair << " ";
	cout << h_area_cn_pair/opt_interval_area << " ";
	cout << "Heuristic CN Pair Displacement "<< h_dist_cn_pair<< " ";
	cout << h_dist_cn_pair/opt_movement_dist << endl;
#endif

	//Closest to c_i pair.
	vector <point *> res_pair = closest_to_prev_heuristic_pair(input);
	vector <point *> h_mid_points_pair = arrange_cg(res_pair);
	vector <point *> h_hull_pair = convex_hull(h_mid_points_pair);
	double h_area_pair = area_of_polygon(h_hull_pair);
	double h_dist_pair = compute_displacement(h_mid_points_pair);
	double r_interval_3 = h_area_pair/opt_interval_area;
	double r_dist_3 = h_dist_pair/opt_movement_dist;

	vector <point *> h_mid_points_pair_2 = arrange_cg_two_selection(res_pair);
	vector <point *> h_hull_pair_2 = convex_hull(h_mid_points_pair_2);
	double h_area_pair_2 = area_of_polygon(h_hull_pair_2);
	double h_dist_pair_2 = compute_displacement(h_mid_points_pair_2);
	double r_interval_6 = h_area_pair_2/opt_interval_area;
	double r_dist_6 = h_dist_pair_2/opt_movement_dist;
#if DEBUG
	print(res_pair);
	cout << "Heuristic Closest to Prev Pair Area "<< h_area_pair << " ";
	cout << h_area_pair/opt_interval_area << " ";
	cout << "Heuristic Closest to Prev Pair Displacement "<< h_dist_pair<< " ";
	cout << h_dist_pair/opt_movement_dist << endl;
#endif

	//Avg Closest to c_n pair.
	vector <point *> avg_res_pair = avg_closest_to_cn_heuristic_pair(input);
	vector <point *> avg_h_mid_points_pair = arrange_cg(avg_res_pair);
	vector <point *> avg_h_hull_pair = convex_hull(avg_h_mid_points_pair);
	double avg_h_area_pair = area_of_polygon(avg_h_hull_pair);
	double avg_h_dist_pair = compute_displacement(avg_h_mid_points_pair);
	double r_interval_4 = avg_h_area_pair/opt_interval_area;
	double r_dist_4 = avg_h_dist_pair/opt_movement_dist;

	vector <point *> avg_h_mid_points_pair_2 = arrange_cg_two_selection(avg_res_pair);
	vector <point *> avg_h_hull_pair_2 = convex_hull(avg_h_mid_points_pair_2);
	double avg_h_area_pair_2 = area_of_polygon(avg_h_hull_pair_2);
	double avg_h_dist_pair_2 = compute_displacement(avg_h_mid_points_pair_2);
	double r_interval_7 = avg_h_area_pair_2/opt_interval_area;
	double r_dist_7 = avg_h_dist_pair_2 / opt_movement_dist;
#if DEBUG
	print(res_pair);
	cout << endl;
	cout << endl;
	print(avg_h_mid_points_pair);
	cout << endl;
	cout << endl;
	print(avg_h_mid_points_pair_2);
	cout << endl;
	cout << endl;
	cout << "Heuristic Closest to Prev Pair Area "<< h_area_pair << " ";
	cout << avg_h_area_pair/opt_interval_area << " ";
	cout << "Heuristic Closest to Prev Pair Displacement "<< h_dist_pair<< " ";
	cout << avg_h_dist_pair/opt_movement_dist << endl;
#endif

	cout << "Interval " << r_interval_0 << " " << r_interval_1 << " " <<r_interval_2 << " " << r_interval_3 << " " << r_interval_4  <<" " << r_interval_5 <<" " << r_interval_6 << " " << r_interval_7<< endl;
	cout << "Movement " << r_dist_0 << " " << r_dist_1 << " " <<r_dist_2 << " " << r_dist_3 << " " << r_dist_4  <<" " << r_dist_5 <<" " << r_dist_6 << " " << r_dist_7<< endl;
	if(FUZZ) assert(r_interval_0 <= 4.0);
	if(FUZZ) assert(r_dist_1 <= 2.0);

	if(r_interval_0 > i_max) i_max = r_interval_0;
	if(r_dist_1 > d_max) d_max = r_dist_1;
#if RANDOM
	for(int i = 0; i < input.size(); ++i){
		delete(input[i]);
	}
	}while(FUZZ && fuzz_count--);
#endif
	cout << "RATIO " << i_max << " " << d_max << endl;
}
