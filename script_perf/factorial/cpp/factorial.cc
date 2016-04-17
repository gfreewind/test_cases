/*
 * =====================================================================================
 *
 *       Filename:  factorial.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/17/2016 09:35:37 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#include <cstdlib>
#include <iostream>

using namespace std;

typedef unsigned long long uint64_t;

uint64_t factorial(int n)
{
	if (n == 0) {
		return 1;
	}

	uint64_t ret = 1;
	for (int i = 1; i <= n; ++i) {
		ret = ret*i;
	}

	return ret;
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		cerr << "Please specify the number which need calculate its factorial" << endl;
		return 1;
	}

	int num = atoi(argv[1]);
	int loops = 1;
	if (argc >= 3) {
		loops = atoi(argv[2]);
	}

	uint64_t ret;
	for (int i = 0; i < loops; ++i) {
		ret = factorial(num);
	}

	cout << ret << endl;
	return 0;
}
