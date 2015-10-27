BACKWARD CHAINING REASONER
CMPT 411 - Jeremy J Hartmann

Operating System: Windows 10
Language: C++
Compiler: GCC v. 4.8.1

COMMAND LINE INSTRUCTIONS:
To compile the program run:
	g++ -std=c++11 Assign2.cpp -o Assign2

NOTE: It is important that you use flag -std=c++11 
as the program uses multiple c++11 features. 

To run the program:
	./Assign2.exe <sometestfile.txt> <arg>

<sometestfile.txt> is the input test file
	i.e. test4.txt etc..

<arg> is either blank, 1 or 2
	<blank> - Run with no additional output. 
	1 - Run with TRACE output
	2 - Run with DEBUG output. 

i.e.
./Assign2.exe test1.txt 2  // Debug level output
./Assign2.exe test1.txt    // No additional output


FEATURES:
**Uses depth first search to solve and search for paths that resolve
**Uses custom tree impl to store the solutions dirvations
**Uses psudo nondeterminsim in the implimentation of Choose() function. (see test5.txt)
**Able to detect recursives loops in the KB and reverse them. (See test7.txt)
 


