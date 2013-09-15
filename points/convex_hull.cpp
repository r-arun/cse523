#include <iostream>
#include <vector>
#include <stack>
#include <cstdlib>
#include <cassert>
#include <ctime>
#include <algorithm>
using namespace std;
class point{
	public:
		int x;
		int y;
	point(){
		x = rand()%20;
		y = rand()%20;
	}
	point(int x1, int y1){
		x = x1;
		y = y1;
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
	point * pivot = v[start];
	int j = start;
	for(int i = start+1; i < limit; ++i){
		int turn = v[ind]->crossproduct(v[i], pivot);
		if(turn > 0){
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
int main(){
	srand(time(NULL));
	/*
	point *p0 = new point(6, 0);
	point *p1 = new point(12, 1);
	point *p2 = new point(11, 2);
	point *p3 = new point(13, 4);
	point *p4 = new point(10, 4);
	point *p5 = new point(9, 5);
	point *p6 = new point(8, 5);
	point *p7 = new point(7, 4);
	point *p8 = new point(6, 3);
	point *p9 = new point(5, 4);
	point *p10 = new point(7, 12);
	point *p11 = new point(4, 4);
	point *p12 = new point(3, 2);
	point *p13 = new point(3, 1);
	cout << p0->crossproduct(p1, p2)<<endl;
	cout << p0->crossproduct(p2, p1)<<endl;
	cout << p0->crossproduct(p3, p4)<<endl;
	cout << p0->crossproduct(p4, p3)<<endl;
	vector<point *> v;
	stack<point *> st;
	v.push_back(p0);
	v.push_back(p6);
	v.push_back(p4);
	v.push_back(p9);
	v.push_back(p1);
	v.push_back(p5);
	v.push_back(p12);
	v.push_back(p11);
	v.push_back(p8);
	v.push_back(p3);
	v.push_back(p7);
	v.push_back(p10);
	v.push_back(p2);
	v.push_back(p13);
	*/
	int sz;
	cin>>sz;
	vector<point *> v;
	stack<point *> st;
	while(sz--){
		v.push_back(new point());	
	}
	cout<<"First "<<endl;
	v[0]->display();
	rearrange_for_lowest(v);
	cout<<"After "<<endl;
	v[0]->display();
	vector <point *> :: iterator it;
	cout<<"Before Sorting"<<endl;
	for(it = v.begin(); it < v.end(); ++it){
		(*it)->display();
	}

	sort(v, 0, 1, v.size());

	cout<<"After Sorting"<<endl;
	for(it = v.begin(); it < v.end(); ++it){
		(*it)->display();
	}
	st.push(v[0]);
	st.push(v[1]);
	st.push(v[2]);
	for(int i = 3; i < v.size(); ++i){
		point *lst = st.top();
		st.pop();
		point * lst1;
		do{
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
	cout << "The convex hull "<<endl;
	while(!st.empty()){
		(st.top())->display();
		poly.push_back(st.top());
		st.pop();
	}
	reverse(poly.begin(), poly.end());
	cout<<"Printing..."<<endl;
	print(poly);
	//cout<<area_of_triangle(p0,p1, p3)<<endl;
	//cout<<area_of_triangle(p3,p10,p12)<<endl;
	//cout<<area_of_triangle(p0,p3, p12)<<endl;
	//cout<<area_of_triangle(p0,p12, p13)<<endl;
	cout<<area_of_polygon(poly)<<endl;
}
