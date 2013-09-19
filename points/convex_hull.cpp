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
#define TEST_BETWEEN 0
#define TEST_FAILED_CASE 0
#define TEST_RANDOM_POINTS 1
#define TEST_CONVEX_HULL 1
#define DEBUG 1
using namespace std;
class point{
	public:
		int x;
		int y;
	point(){
		x = rand()%2000;
		y = rand()%2000;
	}
	point(int x1, int y1){
		x = x1;
		y = y1;
	}
	point * upper_point(){
		return new point(x, y + 2);
	}
	int crossproduct(point * p1, point * p2){
		return (p1->x - x) * (p2->y - y) - (p2->x - x) * (p1->y - y);
	}
	void display(){
		cout<<x<< " "<<y<<endl;
	}
	bool operator< (point & other){
		if(y < other.y)
			return true;
		if(y == other.y && x < other.x)
			return true;
		return false;
	}
	bool between(point * p1, point * p2){
		return ((this->crossproduct(p1, p2) == 0) &&
				((p1->x <= x && x <= p2->x && p1->y <= y && y <= p2->y) ||
				(p2->x <= x && x <= p1->x && p2->y <= y && y <= p1->y) ||
				(p1->x <= x && x <= p2->x && p2->y <= y && y <= p1->y) ||
				(p2->x <= x && x <= p1->x && p1->y <= y && y <= p2->y)));
	}
	double distance(point * other){
		return sqrt(pow(x-other->x,2) + pow(y-other->y,2));
	}
};
void print(vector <point *> v){
	for(vector <point *> :: iterator it = v.begin(); it != v.end(); ++it){
		(*it)->display();
	}
}
void print (vector <point *> v, int start, int limit){
	assert(start < v.size());
	assert(limit <= v.size());
	for(int i = start; i < limit; ++i){
		v[i]->display();
	}
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
		int turn = v[ind]->crossproduct(v[i], pivot);
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
		int a, b;
		inp >> a >> b;
		v.push_back(new point(a, b));
	}
	preprocess_vector(v);
	return  find_convex_hull(v);
}

vector<point *> random_points_test(int size){
	vector<point *> v;
	while(size--){
		v.push_back(new point());	
	}
	preprocess_vector(v);
#if DEBUG
	cout<<"Initial Points "<<endl;
	print(v);	
#endif
	return  find_convex_hull(v);
}
void test_convex_hull(vector <point *> v){
	assert(v[0]->crossproduct(v[v.size()-1],v[1]) < 0);
	for(int i = 1; i < v.size(); ++i){
		 assert(v[i]->crossproduct(v[i-1],v[(i+1)%v.size()]) < 0);
	}
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
#if TEST_RANDOM_POINTS
	int _sz;
	cin>>_sz;
	poly = random_points_test(_sz);
#else
	string str;
	cin>>str;
	poly = read_points_from_file(str);
#endif
	cout<<"Convex Hull"<<endl;
	print(poly);
#if TEST_CONVEX_HULL
	test_convex_hull(poly);
#endif
	cout<<area_of_polygon(poly)<<endl;
	return 0;
}
