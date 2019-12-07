/**
 * File: Tree.h
 * Qiyu Liu
 * Xiaowei Zhu
 * CSE 12, Fall 2019
 * Dec 03 2019
 * cs12fa19ku
 * Assignment Nine
 * 
 * Description:
 * This is the file that contains all the public information of the tree.c file
 * that we would like to show to the world. The hidden file is contained within
 * the .c file.
 */
#ifndef TREE_H
#define TREE_H

/* DO NOT ADD CODE: This file is used in evaluation
 * Changing function signatures will result in a 25% deduction.
 * adding comments is ok.
 */

#include <fstream>
#include <iostream>
using namespace std;

typedef long offset;

template <class Whatever>
struct TNode;

/**
 * Name:
 * Tree<class Whatever>
 * 
 * Description:
 * This file contains the code behind the implementation of a binary search
 * tree. Each tree is composed of nodes. We have some instance variables to
 * store information about the tree. We will outline the main data fields and
 * public functions within this class below.
 *
 * Data Fields:
 * cost -> cost of the Tree
 * fio -> the corresponding file stream
 * operation -> operation of the Tree
 * root -> the root node of the tree
 * occupancy -> how many nodes are in the tree
 * debug_on -> whether we want debug statement
 * tree_count -> number of tree existed
 * 
 * Public functions:
 * Tree -> constructor function 
 * ~Tree -> destructor function
 * GetCost -> return cost
 * GetOperation -> return operation
 * IncrementCost -> increase cost
 * IncrementOperation -> increase operation
 * Set_Debug_On -> set the debug_on TRUE
 * Set_Debug_Off -> set the debug to FALSE
 * Insert -> insert an node storing element within the tree
 * Loopup -> check if an element is stored within the tree
 * Remove -> remove an element from the tree
 * ostream & Write -> write out nodes
 */
template <class Whatever>
class Tree {
        friend struct TNode<Whatever>;
        static long cost;
        static int debug_on;
        fstream * fio;
        long occupancy;
        static long operation;
        offset root;
        unsigned long tree_count;
        void ResetRoot (void);

public:
        Tree (const char *);
        ~Tree (void);

        static long GetCost ();
        static long GetOperation ();
        static void IncrementCost ();
        static void IncrementOperation ();
        static void Set_Debug_On (void);
        static void Set_Debug_Off (void);

        unsigned long Insert (Whatever &);
        unsigned long Lookup (Whatever &) const;
        unsigned long Remove (Whatever &);
        ostream & Write (ostream &) const;
};

#include "Tree.c"

#endif
