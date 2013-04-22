/*The author of this code is Pramod Ganapathi. I am using the code to simiulate the two simulatneous selection case*/
#include <iostream>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <sys/time.h>
#include <boost/random.hpp>
#include <boost/generator_iterator.hpp>
#include <boost/math/distributions.hpp>

#define N 13
#define MAX 100
#define MINIMUM( a, b ) ( ( ( a ) < ( b ) ) ? ( a ) : ( b ) )
#define MAXIMUM( a, b ) ( ( ( a ) > ( b ) ) ? ( a ) : ( b ) )
//#define BESTPERMUTATION 1

using namespace std;
using namespace boost;

enum DISTRIBUTION { UNIFORM, NORMAL, BERNOULLI, EXPONENTIAL };
ofstream out;

double sum( double *a, int k )
{
   double s = 0;

   for ( int i = 0; i < k; i++ )
      s += a[ i ];

   return s;
}

double center( double *a, int k )
{
   return sum( a, k ) / k;
}

class WaiterProblem
{
    public:
    int n, npos, nneg;
    double a[N], pos[N], neg[N], p[N];
    int perm[N], count[N];
    double bestpermutation[N];
    int distribution;
    double maxsum, minsum, maxinterval, mininterval;
    double interval, sum;
    double storelc, storerc;
    double A[ N ], B[ N ], C[ N ], D[ N ];

    // for uniform and non-unform distribution of random numbers
    typedef uniform_int<> UniformDistribution;
    typedef normal_distribution<> NormalDistribution;
    typedef bernoulli_distribution<> BernoulliDistribution;
    typedef exponential_distribution<> ExponentialDistribution;
    typedef mt19937 RandomNumberGenerator;
    RandomNumberGenerator generator;
    typedef variate_generator<RandomNumberGenerator&, UniformDistribution> UniformGenerator;
    typedef variate_generator<RandomNumberGenerator&, NormalDistribution> NormalGenerator;
    typedef variate_generator<RandomNumberGenerator&, BernoulliDistribution> BernoulliGenerator;
    typedef variate_generator<RandomNumberGenerator&, ExponentialDistribution> ExponentialGenerator;

    void Initialize();
    void PrintArray(double arr[], int size, string s);
    void PrintArray(int arr[], int size, string s);
    double Random();
        int RandomUniform();
        double RandomNormal();
        double RandomBernoulli();
        double RandomExponential();
    void CreatePosNegArrays();
        int BinarySearch(int low, int high, double key);
    void CreatePArray();
    void GeneratePermutations(int index);
    void FindMinimumInterval(int index, double average, double lc, double rc);
    void UpdateLeftRightCentroids(int index, double* average, double* lc, double* rc);
    void InitializeMinimumInterval();
    void PrintBestPermutation();
    void PrintIntervalLengths();

    void FindMinimumIntervalSortedPoints();
    void FindMinimumIntervalGreedyInterval();
    void FindMinimumIntervalGreedyIntervalOrigin();
    void FindMinimumIntervalGreedyCentroid();
    void FindMinimumIntervalPositivesNegatives();
    void FindMinimumIntervalGreedyIntervalAllPoints();
        double FindAverage(double average, int n, double value);
        void UpdateIntervalDetails(double point, double* left, double* right, double* interval);
};

double WaiterProblem::Random()
{
    switch (distribution)
    {
        case UNIFORM:
            return RandomUniform();
            break;
        case NORMAL:
            return RandomNormal();
            break;
        case BERNOULLI:
            return RandomBernoulli();
            break;
        case EXPONENTIAL:
            return RandomExponential();
            break;
    }

    return 1.0;
}
int WaiterProblem::RandomUniform()
{
    UniformDistribution distribution(-30, 30);
    UniformGenerator numberGenerator(generator, distribution);
    return numberGenerator();
}
double WaiterProblem::RandomNormal()
{
    NormalDistribution distribution(0.7, 10);
    NormalGenerator numberGenerator(generator, distribution);
    return numberGenerator();
}
double WaiterProblem::RandomBernoulli()
{
    BernoulliDistribution distribution(0.8);
    BernoulliGenerator numberGenerator(generator, distribution);
    return numberGenerator();
}
double WaiterProblem::RandomExponential()
{
    ExponentialDistribution distribution(0.2);
    ExponentialGenerator numberGenerator(generator, distribution);
    return numberGenerator();
}

void WaiterProblem::Initialize()
{
    distribution = NORMAL;

    n = N;
    double tempsum = 0;
    double tempmax = 0;

    for (int i = 0; i < (n-1); i++)
    {
        a[i] = Random();
        tempsum = tempsum + a[i];
        if (fabs(a[i]) > fabs(tempmax))
            tempmax = a[i];
    }
    a[n-1] = -1 * tempsum;
    if (fabs(a[n-1]) > fabs(tempmax))
        tempmax = a[n-1];

    // bound the interval in the range [-1, 1]
    // 1 mandatory, -1 optional
    for (int i = 0; i < n; i++)
        a[i] = a[i] / tempmax;
    sort(a, a + n);

    for (int i = 0; i < n; i++)
        count[i] = 1;

    maxsum = 0; minsum = 100000000; maxinterval = 0; mininterval = 100000000;
    interval = 0; sum = 0;
    //lc = 10000000; rc = -100000000;
}

void WaiterProblem::PrintArray(double arr[], int size, string s)
{
   out << s << ": ";
   for (int i = 0; i < size; i++)
       out << arr[i] << " ";
   out << endl;
}

void WaiterProblem::PrintArray(int arr[], int size, string s)
{
   out << s << ": ";
   for (int i = 0; i < size; i++)
       out << arr[i] << " ";
   out << endl;
}

void WaiterProblem::CreatePosNegArrays()
{
    // the function assumes that array a is sorted
    int index = BinarySearch(0, n-1, 0);

    nneg = index;
    npos = n - index;
    for (int i = index; i < n; i++)
        pos[i - index] = a[i];
    for (int i = index - 1; i >= 0; i--)
        neg[index - 1 - i] = a[i];
}
int WaiterProblem::BinarySearch(int low, int high, double key)
{
    if (low > high)
        return -1;
    else if (low == high)
        return low;

    int mid = (low + high) / 2;

    if (a[mid] < key)
        return BinarySearch(mid + 1, high, key);
    else if (a[mid] > key)
        return BinarySearch(low, mid, key);
    else if (a[mid] == key)
        return mid;
}

void WaiterProblem::CreatePArray()
{
    // this is the array sorted based on the distance from the final centroid
    int i = 0, j = 0, k = 0;
    for (i = 0; i < n;)
    {
        if (j < npos && k < nneg)
        {
            if (fabs(pos[j]) < fabs(neg[k]))
            { p[i] = pos[j]; j++; }
            else
            { p[i] = neg[k]; k++; }

            i++;
        }
        else if (j == npos)
            for (; i < n; i++)
            { p[i] = neg[k]; k++; }
        else
            for (; i < n; i++)
            { p[i] = pos[j]; j++; }
    }
}

void WaiterProblem::GeneratePermutations(int index)
{
	int i;

	if (index == n)
	{
		for (i = 0; i < n; i++)
			out << p[perm[i]] << " ";
		out << endl;
		return;
	}

	for (i = 0; i < n; i++)
	{
		if (count[i])
		{
			--count[i];
			perm[index] = i;

			GeneratePermutations(index + 1);
			++count[i];
		}
	}
}

void WaiterProblem::FindMinimumInterval(int index, double average, double lc, double rc)
{
	int i;

	if (index == n)
	{
        if ((rc - lc) < mininterval)
        {
            mininterval = rc - lc;

            #ifdef BESTPERMUTATION
            for (i = 0; i < n; i++)
            {
                out << p[perm[i]] << " ";
                bestpermutation[i] = p[perm[i]];
            }
            out << "[InterLen = " << (rc - lc) << "]" << endl;
            #endif
        }
		return;
	}

	for (i = 0; i < n; i++)
	{
		if (count[i])
		{
			perm[index] = i;
			double storeaverage = average; double storelc = lc; double storerc = rc;
            UpdateLeftRightCentroids(index, &average, &lc, &rc);
            if ((rc - lc) > mininterval)
            {
                average = storeaverage; lc = storelc; rc = storerc;
                continue;
            }

			--count[i];
			FindMinimumInterval(index + 1, average, lc, rc);
			++count[i];
			average = storeaverage; lc = storelc; rc = storerc;
		}
	}
}

void WaiterProblem::UpdateLeftRightCentroids(int index, double* average, double* lc, double* rc)
{
    *average *= index;
    *average += p[perm[index]];
    *average /= (index + 1);

    if (*average > *rc)
        *rc = *average;
    if (*average < *lc)
        *lc = *average;
}

double WaiterProblem::FindAverage(double average, int n, double value)
{
    return ((average * n) + value) / (n + 1);
}

void WaiterProblem::InitializeMinimumInterval()
{
    double avg = 0, max = 0, min = 0;

    for (int i = 0; i < n; i++)
    {
        avg = avg * i;
        avg = avg + p[i];
        avg = avg / (i + 1);

        if (avg > max)
            max = avg;
        if (avg < min)
            min = avg;
    }

    mininterval = max - min;
}

void WaiterProblem::PrintBestPermutation()
{
    #ifdef BESTPERMUTATION
    for (int i = 0; i < n; i++)
        out << bestpermutation[i] << " ";
    out << "[InterLen = " << mininterval << "]";
    #endif
}
void WaiterProblem::PrintIntervalLengths()
{
    out << "MinimumInterval = " << mininterval << endl;
}

void WaiterProblem::FindMinimumIntervalSortedPoints()
{
    double centroid = 0;
    double lc = 0, rc = 0;
    for (int i = 0; i < n; i++)
    {
        if ((centroid = FindAverage(centroid, i, p[i])) < lc)
            lc = centroid;
        if (centroid > rc)
            rc = centroid;
    }
    out << "MinimumIntervalSortedPoints = " << (rc - lc) << endl;
}

void WaiterProblem::FindMinimumIntervalGreedyInterval()
{
    double store = 0;
    double centroid = 0, pcentroid = 0, ncentroid = 0;
    double lc = 0, rc = 0;
    double length, plength = 0, nlength = 0;
    int i = 0, j = 0, nc = 0;
    double midpoint = 0;

    while (i < npos && j < nneg)
    {
        plength = nlength = 0;

        pcentroid = FindAverage(centroid, nc, pos[i]);
        if (pcentroid > rc)
            plength = pcentroid - lc;

        ncentroid = FindAverage(centroid, nc, neg[j]);
        if (ncentroid < lc)
            nlength = rc - ncentroid;

        if (plength < nlength)
        { centroid = pcentroid; i++; }
        else if (plength > nlength)
        { centroid = ncentroid; j++; }
        else
        {
            //midpoint = (rc - lc) / 2;
            midpoint = 0;
            if (fabs(pcentroid - midpoint) < fabs(ncentroid - midpoint))
            { centroid = pcentroid; i++; }
            else
            { centroid = ncentroid; j++; }
        }

        UpdateIntervalDetails(centroid, &lc, &rc, &length);
        nc++;
    }

    for (;i < npos; i++)
    {
        centroid = FindAverage(centroid, nc, pos[i]);
        UpdateIntervalDetails(centroid, &lc, &rc, &length);
        nc++;
    }
    for (;j < nneg; j++)
    {
        centroid = FindAverage(centroid, nc, neg[j]);
        UpdateIntervalDetails(centroid, &lc, &rc, &length);
        nc++;
    }
    out << "MinimumIntervalGreedyInterval = " << (rc - lc) << endl;
}

void WaiterProblem::FindMinimumIntervalGreedyCentroid()
{
    double store = 0;
    double centroid = 0, pcentroid = 0, ncentroid = 0;
    double lc = 0, rc = 0;
    double length, plength = 0, nlength = 0;
    int i = 0, j = 0, nc = 0;

    while (i < npos && j < nneg)
    {
        pcentroid = FindAverage(centroid, nc, pos[i]);
        ncentroid = FindAverage(centroid, nc, neg[j]);

        if (fabs(pcentroid) < fabs(ncentroid))
        { centroid = pcentroid; i++; }
        else
        { centroid = ncentroid; j++; }

        UpdateIntervalDetails(centroid, &lc, &rc, &length);
        nc++;
    }

    for (;i < npos; i++)
    {
        centroid = FindAverage(centroid, nc, pos[i]);
        UpdateIntervalDetails(centroid, &lc, &rc, &length);
        nc++;
    }
    for (;j < nneg; j++)
    {
        centroid = FindAverage(centroid, nc, neg[j]);
        UpdateIntervalDetails(centroid, &lc, &rc, &length);
        nc++;
    }
    out << "MinimumIntervalGreedyCentroid = " << (rc - lc) << endl;
}

void WaiterProblem::UpdateIntervalDetails(double point, double* left, double* right, double* interval)
{
    if (point > *right)
        *right = point;
    if (point < *left)
        *left = point;
    *interval = *right - *left;
}

void WaiterProblem::FindMinimumIntervalPositivesNegatives()
{
    double minimum = 1000000000;
    int i = 0, j = 0, nc = 0;
    double centroid = 0, pcentroid = 0, ncentroid = 0, length = 0, lc = 0, rc = 0;

    while (j < nneg)
    {
        ncentroid = FindAverage(centroid, nc, neg[j]);
        if (ncentroid >= 0)
        {
            centroid = ncentroid;
            UpdateIntervalDetails(centroid, &lc, &rc, &length);
            j++;
        }
        else
        {
            centroid = FindAverage(centroid, nc, pos[i]);
            UpdateIntervalDetails(centroid, &lc, &rc, &length);
            i++;
        }
        nc++;
    }

    while (j < nneg)
    {
        centroid = FindAverage(centroid, nc, neg[j]);
        UpdateIntervalDetails(centroid, &lc, &rc, &length);
        j++; nc++;
    }
    minimum = (rc - lc);

    i = 0; j = 0; nc = 0;
    centroid = 0; pcentroid = 0; ncentroid = 0; length = 0; lc = 0; rc = 0;

    while (i < npos)
    {
        pcentroid = FindAverage(centroid, nc, pos[i]);
        if (pcentroid <= 0)
        {
            centroid = pcentroid;
            UpdateIntervalDetails(centroid, &lc, &rc, &length);
            i++;
        }
        else
        {
            centroid = FindAverage(centroid, nc, neg[j]);
            UpdateIntervalDetails(centroid, &lc, &rc, &length);
            j++;
        }
        nc++;
    }

    while (i < npos)
    {
        centroid = FindAverage(centroid, nc, pos[i]);
        UpdateIntervalDetails(centroid, &lc, &rc, &length);
        i++; nc++;
    }

    minimum = MINIMUM(minimum, (rc - lc));
    out << "MinimumIntervalPositivesNegatives = " << minimum << endl;
}

void WaiterProblem::FindMinimumIntervalGreedyIntervalAllPoints()
{
    int i;
    double c = 0, cmin = 0, cmax = 0;

    //while (1)
    for (i = 0; i < n;)
    {
        for (i = 0; i < n; i++)
            C[i] = p[i];

        for (i = 0; i < n; i++)
        {
            int k = -1, l = -1;
            double lc = 0;

            for (int j = 0; j < n - i; j++)
            {
                B[i] = C[j];
                double cc = center(B, i + 1);
                if ((cc < cmin) || (cc > cmax))
                {
                    if ((l == -1 ) || ( min( fabs( cc - cmin ), fabs( cc - cmax ) ) < min( fabs( lc - cmin ), fabs( lc - cmax ) )))
                    { l = j; lc = cc; }
                    continue;
                }
                if ((k == -1) || (fabs(p[ j ] - c) > fabs(p[ k ] - c)))
                    k = j;
            }

            if (k == -1)
            {
                if (lc < cmin) cmin = lc;
                if (lc > cmax) cmax = lc;
                break;
            }
            else
            {
                B[i] = C[k];
                C[k] = C[n - i - 1];
                double cc = center(B, i + 1);
            }
        }
        //if (i == n) break;
    }

    out << "MinimumIntervalGreedyIntervalAllPoints = " << (cmax - cmin) << endl;
}

int main()
{
    WaiterProblem wp;
    wp.generator.seed(std::time(0));
    out.open("waiterproblem.txt");

    for (int run = 1; run < 40; run++)
    {
        wp.Initialize();
        wp.PrintArray(wp.a, wp.n, "Array");
        wp.CreatePosNegArrays();
        //wp.PrintArray(wp.pos, wp.npos, "Positive");
        //wp.PrintArray(wp.neg, wp.nneg, "Negative");
        wp.CreatePArray();
        //wp.PrintArray(wp.p, wp.n, "Sorted Array");
        //out << endl;

        wp.InitializeMinimumInterval();
        wp.FindMinimumInterval(0, 0, 0, 0);
        wp.PrintBestPermutation();
        wp.PrintIntervalLengths();
        wp.FindMinimumIntervalGreedyIntervalAllPoints();
        wp.FindMinimumIntervalGreedyInterval();
        wp.FindMinimumIntervalGreedyCentroid();
        wp.FindMinimumIntervalPositivesNegatives();
        wp.FindMinimumIntervalSortedPoints();

        out << endl;
    }

    return 0;
}
