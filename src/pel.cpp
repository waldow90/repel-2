//============================================================================
// Name        : pel.cpp
// Author      : Joe Selman
// Version     :
// Copyright   : 
// Description : Hello World in C++, Ansi-style
//============================================================================

#ifdef HAVE_CONFIG_H
  #include "../config.h"
#endif
#include <iostream>
#include "interval.h"

using namespace std;

int main() {
	Interval blarg(0,5);
	std::cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	char x;
	std::cin >> x;
	return 0;
}