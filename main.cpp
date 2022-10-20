//Include headers that ship as part of C++ / are standard

#include <stdio.h>		//File IO
#include <iostream>		//Stream IO incl std out/err


//Include custom headers

// Main function 

int main(int argc, char* argv[]) {

	// Write to standard output
	std::cout << "Hello, World!" << std::endl;

	// Return zero on succesful program run (good practice, aids error 
	// reporting if running as a subprocess)
	return 0;
}


