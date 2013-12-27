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
#include "convex_hull.h"

#define TEST_BETWEEN 0
#define TEST_FAILED_CASE 0
#define TEST_RANDOM_POINTS 1
#define TEST_CONVEX_HULL 1
#define DEBUG 1

using namespace std;
void test_between(){
	point *p0 = new point(3, 3);
	point *p1 = new point(4, 4);
	point *p2 = new point(5, 5);
	point *p3 = new point(10, 10);
	point *p4 = new point(9, 11);
	point *p5 = new point(8, 12);
	point *p6 = new point(7, 13);
	point * p7 = p2->upper_point();
	//positive slope
	assert(p2->crossproduct(p1, p0) == 0);
	assert(p1->between(p0,p2));
	assert(!p3->between(p0,p2));
	assert(p2->between(p0,p3));
	//non-collinear point
	assert(!p7->between(p0,p3));
	//negative slope
	assert(p4->between(p3,p5));
	assert(!p6->between(p3,p5));
	assert(p5->between(p3,p6));

	point * p10 = new point(2,12);
	point * p11 = new point(1,11);
	point * p12 = new point(12,19);
	cout<<p10->crossproduct(p11, p12)<<endl;
	assert(p10->crossproduct(p11, p12) > 0);

	point * p20 = new point(1,2);
	point * p21 = new point(0,9);
	point * p22 = new point(3,0);
	cout<<p20->crossproduct(p21, p22)<<endl;
	assert(p20->crossproduct(p21, p22) > 0);
}

void test_failed_case(){
	point *p0 = new point(2, 19);
	point *p1 = new point(17, 15);
	point *p2 = new point(7, 5);
	point *p3 = new point(9, 8);
	point *p4 = new point(8, 12);
	point *p5 = new point(0, 6);
	point *p6 = new point(1, 9);
	point *p7 = new point(15, 3);
	vector <point *> v;
	v.push_back(p0);
	v.push_back(p1);
	v.push_back(p2);
	v.push_back(p3);
	v.push_back(p4);
	v.push_back(p5);
	v.push_back(p6);
	v.push_back(p7);
	rearrange_for_lowest(v);
	sort(v, 0, 1, v.size());
	vector <point *> poly  = find_convex_hull(v);
	cout << "The convex hull "<<endl;
	print(poly);
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
vector <point *> initial;
vector<point *> random_points_test(int size){
	vector<point *> v;
	while(size--){
		v.push_back(new point());	
	}
	initial = v;
#if DEBUG
	print(initial);	
#endif
	preprocess_vector(v);
	return  find_convex_hull(v);
}
void test_convex_hull(vector <point *> v){
	assert(v[0]->crossproduct(v[v.size()-1],v[1]) < 0);
	for(int i = 1; i < v.size(); ++i){
		 assert(v[i]->crossproduct(v[i-1],v[(i+1)%v.size()]) < 0);
	}
}
void print_special(vector <point *>poly, vector<point*> hull){
	for(int i = 0; i < hull.size(); ++i){
		cout << find(poly.begin(), poly.end(), hull[i])-poly.begin() <<" ";
	}
	cout << endl;
}
int main(){
	srand(time(NULL));
	vector<point *> poly;
#if TEST_BETWEEN
	test_between();
#endif
#if TEST_FAILED_CASE
	test_failed_case();
#endif
	vector <point *> hull;
#if TEST_RANDOM_POINTS
	int _sz;
	cin>>_sz;
	hull = random_points_test(_sz);
#else
	string str;
	cin>>str;
	hull = read_points_from_file(str);
#endif
	cout<<"Convex Hull"<<endl;
//	print(hull);
	print_special(initial,hull);
#if TEST_CONVEX_HULL
	test_convex_hull(hull);
#endif
	clock_t c1_start = clock();
	double a1  = area_of_polygon(hull);
	clock_t c1_end = clock();
	clock_t c2_start = clock();
	double a2  = area_of_convex_polygon(hull);
	clock_t c2_end = clock();
	cout << endl << a1 << " " << a2 << endl;
	cout << double(c1_end - c1_start)/CLOCKS_PER_SEC <<" " <<double(c2_end - c2_start)/CLOCKS_PER_SEC  << endl;
	assert(a1 == a2);
	return 0;
}
