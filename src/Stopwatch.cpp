/*
Stopwatch.cpp: a basic stopwatch to time runs
*/
#include "Stopwatch.h"

using namespace std;

void Stopwatch::reset() {
	start = chrono::steady_clock::now();
}

double Stopwatch::elapsed_ms() {
	return chrono::duration <double, milli> (chrono::steady_clock::now() - start).count();
}
