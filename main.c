#include <stdio.h>
#include <time.h>

#include "FOVc.h"
#include "mtwist.h"

void base();
void test1();
void test2();
void test3();
void test4();

double calcSeconds (clock_t start, clock_t finish);
clock_t startTest;
clock_t endTest;
clock_t timePassed;
double seconds;
const bool USE_TIMER = true;		// Calculates time to run each test case


int NUM_ITERATIONS = 1;			// Number of times the test function is run
								    // when calculating LOS
int NUM_RUNS = 1;					// How many times to run the test code
const int X_RANGE = 1000;			// Horizontal map size, from 0 to X_RANGE
const int Y_RANGE = 100;			// Vertical map size, from 0 to Y_RANGE

int main()
{
	mt_seed32new(mt_seed());

	int a;
	int mult;


	for (mult = 0; mult < 1; mult++)
	{

		// NUM_ITERATIONS += 25;


		//printf ("Configuration: OFFSET coordinate system.\n");
		printf ("Number of iterations: %d\n", NUM_ITERATIONS);

		for (a = 0; a < NUM_RUNS; a++)
		{
			printf ("\n\n\n--------------RUN #%d-----------\n", a+1);
			base();
			test1();
			test2();
			test3();
			test4();
		}

	}
	return 0;
}

void base()
{
	int base;
	int belongToUs = 0;
	int a;

	if (USE_TIMER) startTest = clock();

	//printf ("Starting base function.\n");

	for (a = 0; a < NUM_ITERATIONS; a++)
	{
		for (base = 0; base < 1800000000; base++)
			belongToUs ++;
	}

	if (USE_TIMER) {
		endTest = clock();
		printf ("Base: %0.2f seconds.\n", calcSeconds (startTest, endTest));
	} else printf ("Base function completed.\n");
}

void test1()
{
	int i;

	if (USE_TIMER) startTest = clock();

	//printf ("TEST ONE: Line of Sight calculations between two iterated points\n");

	for (i = 0; i < NUM_ITERATIONS; i++)
		ScanLOSCalc(X_RANGE, Y_RANGE);

	if (USE_TIMER) {
		endTest = clock();
		printf ("TEST ONE: %0.2f seconds.\n", calcSeconds (startTest, endTest));
	} else printf ("Test one completed.\n\n");
}

void test2()
{
	int i;

	if (USE_TIMER) startTest = clock();

	//printf ("TEST TWO: Line of Sight calculations between two random points\n");

	for (i = 0; i < NUM_ITERATIONS; i++)
		RandLOSCalc(X_RANGE, Y_RANGE, X_RANGE * Y_RANGE);

	if (USE_TIMER) {
			endTest = clock();
			printf ("TEST TWO: %0.2f seconds.\n", calcSeconds (startTest, endTest));
	} else printf ("Test two completed.\n\n");
}

void test3()
{
	int i;

	if (USE_TIMER) startTest = clock();

	//printf ("TEST THREE: Only does distance and azimuth calculations between two random points.\n");

	for (i = 0; i < NUM_ITERATIONS; i++)
		RandLOSCalcMin(X_RANGE, Y_RANGE, X_RANGE * Y_RANGE * 400);

	if (USE_TIMER) {
			endTest = clock();
			printf ("TEST THREE: %0.2f seconds.\n", calcSeconds (startTest, endTest));
	} else printf ("Test three completed.\n\n");
}

void test4()
{
	int i;

	if (USE_TIMER) startTest = clock();

	//printf ("TEST FOUR: Prints random coordinates in even-r offset format.\n");

	for (i = 0; i < NUM_ITERATIONS; i++)
		RandDisplayCoord(X_RANGE, Y_RANGE, X_RANGE * Y_RANGE * 1250);

	if (USE_TIMER) {
			endTest = clock();
			printf ("TEST FOUR: %0.2f seconds.\n", calcSeconds (startTest, endTest));
	} else printf ("Test four completed.\n\n");
}

double calcSeconds (clock_t start, clock_t finish)
{
	return (finish - start) / (double)(CLOCKS_PER_SEC);
}
