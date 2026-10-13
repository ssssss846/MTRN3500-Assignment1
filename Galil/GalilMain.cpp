#include "Galil.h"

#include <cstdio>
#include <iostream>
#include <string>
#include <cmath>

#include <chrono>
#include <thread>

using namespace std::chrono_literals;

#using <System.dll>	
using namespace System;
using namespace System::Threading;


int main(void) {
	EmbeddedFunctions funcs(true);   // simulator
	Galil myGalil(&funcs, "192.168.0.120 -d");

	return 0;
}