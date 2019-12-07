/**
 * File: Driver.c
 * Qiyu Liu
 * Xiaowei Zhu
 * CSE 12, Fall 2019
 * Dec 3 2019
 * cs12fa19ku
 * Assignment Nine 
 * 
 * Description:
 * This contains the main Driver information for running the program. It 
 * contains all the prompts and switch statements in order for the program
 * to run. In the end, it also prints out the cost and operation of the tree.
 * 
 */
#include <iostream>
#include <cstdio>
#include <string>
#include <getopt.h>
#include "Driver.h"
#include "SymTab.h"
#include <fstream>

using namespace std;

#ifdef NULL
#undef NULL
#define NULL 0
#endif

ostream & operator << (ostream & stream, const UCSDStudent & stu) {
	return stream << "name:  " << stu.name
		<< " with studentnum:  " << stu.studentnum;
}

/**
 * Name: main
 * all the actual execution of the program happens here. It contains switch
 * statement and prompts for user interation. User input is taken in using
 * cin. In the end of the program, cost and operation are printed.
 *
 * @param   argc int type -> number of command line argument
 * @param 	argv const char * ->  point to the argument strings
 * @return  int type
 */
int main (int argc, char * const * argv) {
	char buffer[BUFSIZ];
	char command;
	long number;
	char option;
	// istream and ostream pointer for file
	istream * inStream = &cin;
	ostream * outStream = &cout; 

	SymTab<UCSDStudent>::Set_Debug_Off ();

	while ((option = getopt (argc, argv, "x")) != EOF) {

		switch (option) {
			case 'x': SymTab<UCSDStudent>::Set_Debug_On ();
					  break;
		}       
	}

	SymTab<UCSDStudent> ST("Driver.datafile");
	ST.Write (cout << "Initial Symbol Table:\n" );

	while (cin) {
		// checking for EOF
		if (!(*inStream)){
			// deleting pointers and reassigning then
			delete inStream;
			inStream = &cin;
			delete outStream;
			outStream = &cout;
		}
		command = NULL;         // reset command each time in loop
		// prompting for user input
		*outStream << "Please enter a command ((f)ile, (i)nsert, "
			<< "(l)ookup, (r)emove, (w)rite):  ";
		*inStream >> command;

		switch (command) {

			case 'i': {
				// prompting for input
				*outStream << "Please enter UCSD student name to insert:  ";
				*inStream >> buffer;  // formatted input

				// prompting for input
				*outStream << "Please enter UCSD student number:  ";
				*inStream >> number;

				UCSDStudent stu (buffer, number);

				// create student and place in symbol table
				ST.Insert (stu);
				break;
			}
			case 'l': { 
				unsigned long found;    // whether found or not

				// prompting for input
				*outStream << "Please enter UCSD student name to lookup:  ";
				*inStream >> buffer;  // formatted input

				UCSDStudent stu (buffer, 0);
				found = ST.Lookup (stu);

				// statement for whether the student is found
				if (found)
					cout << "Student found!!!\n" << stu << "\n";
				else
					cout << "student " << buffer << " not there!\n";
				break;
			}
			case 'r': { 
				unsigned long removed;

				// prompting for input
				*outStream << "Please enter UCSD student name to remove:  ";
				*inStream >> buffer;  // formatted input

				UCSDStudent stu (buffer, 0);
				removed = ST.Remove(stu);

				if (removed)
					cout << "Student removed!!!\n" << stu << "\n";
				else
					cout << "student " << buffer << " not there!\n";
				break;
			}
			case 'w':
				// delegate to the Write method to write out
				ST.Write (cout << "The Symbol Table contains:\n");
				break;
			case 'f':
			// handling input from file

				// prompting for user input
				*outStream << "Please enter file name for commands:  ";
				*inStream >> buffer; // formatted input

				// end of command input file
				if (inStream != &cin){
					delete inStream;
					delete outStream;
				}

				// command such as f newfile within command input file
				inStream = new ifstream (buffer);
				outStream = new ofstream("/dev/null");
				break;
		}
	}

	ST.Write (cout << "\nFinal Symbol Table:\n");
	
	// print outs information about cost and operation
	if (ST.GetOperation() != 0) {
		cout << "\nCost of operations:    ";
		cout << ST.GetCost();
		cout << " tree accesses";

		cout << "\nNumber of operations:  ";
		cout << ST.GetOperation();

		cout << "\nAverage cost:          ";
		cout << (((float)(ST.GetCost()))/(ST.GetOperation()));
		cout << " tree accesses/operation\n";
	}

	else
		cout << "\nNo cost information available.\n";

}
