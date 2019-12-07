/**
 * File: Driver.h
 * Qiyu Liu 
 * Xiaowei Zhu
 * CSE 12, Fall 2019
 * Nov 26 2019
 * cs12fa19ku
 * Assignment Nine 
 * 
 * Description:
 * This file contains the code for UCSDStudent class and the main driver for the
 * tree. The detail description for the UCSDStudent class is outlined in
 * its class header. The primary purpose of the main driver is to test if our
 * binary search tree is doing what it supposes to do.
 * 
 */
#ifndef DRIVER_H
#define DRIVER_H

#include <string.h>
#include <iostream>
#include <cstdlib>
using namespace std;

/**
 * Name:
 * UCSDStudent.c
 * 
 * Description:
 * This contains the code for the UCSDStudent class. We have two instance
 * varibales. The equals and isGreaterThan function will be used in the tree
 * to for comparing the node. The toString method will be used in the write
 * method to write out the information. The getName method will return the name.
 * 
 * Data Fields:
 * name: pointer to the name field of the student 
 * studentnum: the student number
 * 
 * Public functions:
 * UCSDStudent -> constructor function
 * operator == -> check if two objects are equal
 * operator < -> check if one objects is less than the other
 * const char * -> return the name of the student
 *
 */
class UCSDStudent {
	friend ostream & operator << (ostream &, const UCSDStudent &);
	char name[16];
	long studentnum;
	public:

	/**
	* Name: UCSDStudent 
	* Purpose and description:
	* This constructor initializes the data memory associated with the 
	* UCSDStudent object.
	* 
	* @param  nm -> the name of the student
	* @param  val -> the number associated with the student
	*/
	UCSDStudent (char * name , long val = 0): studentnum (val) {
		memset (this->name, '\0', sizeof (this->name));
		strcpy(this -> name, name);
	}

	/**
	* Name: UCSDStudent 
	* Purpose and description:
	* This is the copy constructor for UCSDStudent class.
	* 
	* @param stu UCSDStudent -> reference to the one to clone
	*/
	UCSDStudent (const UCSDStudent & stu) {
		memset (name, '\0', sizeof (name));
		strcpy (name, stu.name);
		studentnum = stu.studentnum;
	}

	/**
	* Name: UCSDStudent 
	* Purpose and description:
	* This is the default constructor for UCSDStudent class.
	* 
	*/
	UCSDStudent(void) : studentnum(0) {
		memset(name, '\0', sizeof(name));
	}

	/**
	* Name: operator const char * 
	* Purpose and description:
	* Returns the tree's name
	* @return name of the student 
	*/
	operator const char * (void) const {
		return name;
	}

	/**
	* Name: operator ==  
	* Purpose and description:
	* check if the two objects are objects with the same name. 
	* @param base -> the object for comparison
	* @return long for indication of success 
	*/
	long operator == (const UCSDStudent & ucsdstudent) const {
		return ! strcmp (name, ucsdstudent.name);
	}

	/**
	* Name: operator <
	* Purpose and description:
	* check if the one object is lesser than the other object. 
	* @param Bse -> the object for comparison
	* @return long for indication of success 
	*/
	long operator < (const UCSDStudent & ucsdstudent) const {
		return (strcmp (name, ucsdstudent.name) < 0) ? 1 : 0;
	}
};

#endif
