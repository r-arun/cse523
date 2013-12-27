#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stack>
#include <cstdlib>
#include <cassert>
#include <ctime>
#include <cmath>
#include <algorithm>
#include "point.h"
using namespace std;

void print(vector <point *> v){
	for(vector <point *> :: iterator it = v.begin(); it != v.end(); ++it){
		cout << (*it)->x << " " << (*it)->y << endl;
	}
}
void print (vector <point *> v, int start, int limit){
	assert(start < v.size());
	assert(limit <= v.size());
	for(int i = start; i < limit; ++i){
		v[i]->display();
	}
}
point * center_of_mass(vector <point *> v){
	point *m = new point(0, 0);
	int cnt = 0;
	for(vector <point *> :: iterator vin = v.begin(); vin!=v.end(); ++vin){
		++cnt;
		m->x += (*vin)->x;
		m->y += (*vin)->y;
	}
	return new point(m->x/cnt, m->y/cnt);
}
void rearrange_for_lowest(vector <point *> & v){
	point * low = v[0];
	int pos = 0;
	for(int i = 1; i < v.size(); ++i)
	{
		if(v[i]->operator<(*low)){
			pos = i;
			low = v[i];
		}
	}
	v[pos] =  v[0];
	v[0] = low;
}

int partition(vector <point *> & v, int ind, int start, int limit){
	//Sort by angle :counter-clockwise first
	//If angles are equal, sort by distance from v[ind]
	point * pivot = v[start];
	int j = start;
	for(int i = start+1; i < limit; ++i){
		float turn = v[ind]->crossproduct(v[i], pivot);
		if(turn >= 0){
			if(turn == 0){
				//put the closer point first
				//our removal method for duplicates with same angle
				//retains only the farthest point. So if angles are the same
				//we then sort by closest point
				if(v[ind]->distance(v[i]) > v[ind]->distance(pivot)){
					continue;
				}
			}
			++j;
			point * temp = v[i];
			v[i] = v[j];
			v[j] = temp;
		}
	}
	v[start] = v[j];
	v[j] = pivot;
	assert(j<limit);
	return j;
}

void sort(vector<point *> & v, int ind, int start, int limit){
	//ind is the index of the pivot	
	if(start+1 >= limit) return;
	int pivot = partition(v, ind, start, limit);
	sort(v, ind, start, pivot);
	sort(v, ind, pivot + 1, limit);
}

#define abs(x) x>0?x:-x
double area_of_triangle(point * p1, point * p2, point * p3){
	return abs((p2->x * p1->y - p1->x * p2->y + p3->x * p2 -> y -
			p2->x * p3->y + p1->x * p3->y - p3 ->x * p1->y)/2);
}

double area_of_polygon(vector<point *> v){
	if(v.size() < 3){
		//happens when you split a quadrilateral. Does not contain an area.
		return 0.0;
	}
	if(v.size() == 3){
		return area_of_triangle(v[0], v[1], v[2]);
	}
	double area = 0.0;
	vector<point *> poly;
	for(int i = 0; i < v.size(); i+=2){
		if(i+2 <= v.size())
			area += area_of_triangle(v[i % v.size()], v[(i+1) % v.size()],
													v[(i+2) % v.size()]);
		poly.push_back(v[i]);
	}
	return area += area_of_polygon(poly);
}

double area_of_convex_polygon(vector <point *> v){
	v.push_back(v[0]);
	double a = 0.0;
	double b = 0.0;
	for(int i = 0; i+1 < v.size(); ++i){
		a += v[i]->x * v[i+1]->y;
	}
	for(int i = 0; i+1 < v.size(); ++i){
		b += v[i]->y * v[i+1]->x;
	}
	return (a-b)/2;
}

void remove_duplicate_points(vector <point *> & v){
	if(v.size() < 3) return;
	for(int i = 1; i < v.size()-1; ++i){
		if(v[i]->between(v[0],v[i+1])){
			v.erase(v.begin() + i);	
			--i;
		}
	}
}

void preprocess_vector(vector <point *> & v){
	rearrange_for_lowest(v);	
	sort(v, 0, 1, v.size());
	remove_duplicate_points(v);	
}

vector<point *> find_convex_hull(vector<point *> v){
	if(v.size() < 3) return v;
	stack<point *> st;
	st.push(v[0]);
	st.push(v[1]);
	st.push(v[2]);
	for(int i = 3; i < v.size(); ++i){
		point *lst = st.top();
		st.pop();
		point * lst1;
		do{
			assert(!st.empty());
			lst1 = st.top();
			st.pop();
			if(lst1->crossproduct(lst, v[i]) <= 0){
				lst = lst1;
			}
			else{
				st.push(lst1);
				st.push(lst);
				st.push(v[i]);
				break;	
			}
		}while(true);
	}
	vector <point *> poly;
	while(!st.empty()){
		poly.push_back(st.top());
		st.pop();
	}
	reverse(poly.begin(), poly.end());
	return poly;
}

vector <point *> convex_hull(vector <point *> p){
	preprocess_vector(p);
	return find_convex_hull(p);
}
