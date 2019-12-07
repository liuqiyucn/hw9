/**
 * File: Tree.c
 * Qiyu Liu
 * Xiaowei Zhu
 * CSE 12, Fall 2019
 * Dec 03 2019
 * cs12fa19ku
 * Assignment Nine
 * 
 * Description:
 * This file contains the algorithm for the implementation of a binary tree data
 * structure. Each tree is composed by nodes. We have some instance variables to
 * store information about the tree. Each node of the tree will have a left and
 * right children. Data is stored within the node. This data
 * structrue will be used to allow variable assignment. Everything is
 * implemented recursively. 
 */
#include <stdlib.h>
#include <string.h>
#include "Tree.h"

// debug messages
static const char ALLOCATE[] = " - Allocating]\n";
static const char COST_READ[] = "[Cost Increment (Disk Access): Reading ";
static const char COST_WRITE[] = "[Cost Increment (Disk Access): Writing ";
static const char DEALLOCATE[] = " - Deallocating]\n";
static const char TREE[] = "[Tree ";
static const char RIGHT_BRACKET[] = "]\n"; // right bracket and newline
static int counter = 0; // counter for tree_count

template <class Whatever>
int Tree<Whatever>::debug_on = 0;

template <class Whatever>
long Tree<Whatever>::cost = 0;

template <class Whatever>
long Tree<Whatever>::operation = 0;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define THRESHOLD 2

template <class Whatever>
ostream & operator << (ostream &, const TNode<Whatever> &);

/**
 * Name:
 * TNode.c
 * 
 * Description:
 * This struct define the necessary data fields and methods for a node inside a
 * binary search tree. The detail description of the data fields and public
 * method is included below.
 *
 * Data Fields:
 * data -> the information stored within the node
 * left -> left child of the node
 * right -> right child of the node
 * height -> 1+height of the tallest child, or 0 for leaf
 * balance -> left child's height - right child's height
 * this_position -> current position of the TNode
 * 
 * Public functions:
 * TNode -> create a new TNode
 * TNode -> add new node to disk
 * TNode -> read node from disk
 * Write -> update node to disk
 * Read -> read node from disk
 * writeAllTNodes -> write out all the nodes and their data
 * ReplaceAndRemoveMin -> remove the minimum child
 * SetHeightAndBalance -> set height and balance of the node
 * Remove -> remove the node
 * Insert -> insert the node
 * Lookup -> Lookup the node
 */
template <class Whatever>
struct  TNode {
	// friends:

	// data fields:
	Whatever data;
	long height;
	long balance;
	offset left;
	offset right;
	offset this_position;	// current position
	// function fields:
	TNode () : height (0), balance (0), left (0), right (0), 
	this_position (0) {}

	// to declare the working TNode in Tree's Remove
	TNode (Whatever & element) : data (element), height (0), balance (0),
	left (0), right (0), this_position (0) {}

	TNode (Whatever &, fstream *, long &);	// to add new node to disk
	TNode (const offset &, fstream *);	// to read node from disk

	unsigned long Insert (Whatever &, fstream *, long &, offset &);
	// optional recursive Lookup declaration would go here
	void Read (const offset &, fstream *);	// read node from disk
	unsigned long Remove (TNode<Whatever> &, fstream *, long &, offset &,
			long fromSHB = FALSE);
	void ReplaceAndRemoveMin (TNode<Whatever> &, fstream *, offset &);
	void SetHeightAndBalance (fstream *, offset &);
	void Write (fstream *) const;		// update node to disk

	ostream & Write_AllTNodes (ostream &, fstream *) const;

	/**
	 * Name: Lookup 
	 * Purpose and description:
	 * This method looks up an element from the tree.
	 * 
	 * @param element Whatever type -> the data to be inserted
	 * @param fio fstream * type -> pointer to the data file on disk
	 * @return TRUE for success, FALSE for failure
	 */
	unsigned long Lookup(Whatever & element, fstream * fio) const {
		// storing return value
		unsigned long returnVal; 

		// found the item
		if (data == element){
			element = data;
			returnVal = TRUE;
		}
		// going left if the element if smaller
		else if (element < data){
			if (!left){
				return FALSE;
			}
			TNode<Whatever> lChild (left, fio);
			returnVal = lChild.Lookup(element, fio);
		}
		// going right otherwise
		else{
			if (!right){
				return FALSE;
			}
			TNode<Whatever> rChild (right, fio);
			returnVal = rChild.Lookup(element, fio);
		}
		// return statement
		return returnVal;
	}
};

/**
 * Name: Tree<Whatever> :: Insert (public)
 * Performs insertion into the binary search tree. It will return true if the
 * insertion is successful, and false if the insertion is unseccessful.
 *
 * @param   Whatever type -> the element to be inserted 
 * @return  unsigned long type -> TRUE for success, FALSE for unsuccess 
 */
template <class Whatever>
unsigned long Tree<Whatever> :: Insert (Whatever & element) {
	IncrementOperation();
	// empty tree case
	if (!occupancy){
		// Writes a newNode to disk
		TNode<Whatever> newNode (element, fio, occupancy);
		// set the root data field to the newNode's position on disk 
		root = newNode.this_position;
	}
	// not empty tree, delegate to TNode's insert
	else {
		// read the rootNode and use that to call Insert
		TNode<Whatever> rootNode (root, fio);
		rootNode.Insert (element, fio, occupancy, root);
	}
	return TRUE;
}

/**
 * Name: ReplaceAndRemoveMin 
 * Purpose and description:
 * This method removes an element with two children. 
 * 
 * @param targetTNode TNode<Whatever> -> the data to be inserted
 * @param fio fstream * type -> filestream to the datafile on disk
 * @param PositionInParent TNode<Whatever> type -> reference to Parent's
 *		position
 */
template <class Whatever>
void TNode<Whatever> :: ReplaceAndRemoveMin (TNode<Whatever> & targetTNode, 
		fstream * fio, offset & PositionInParent) {
		// if this is not null
		if (this){
			// if there is left child, recursively go left
			if (left){
				TNode<Whatever> lChild (left, fio);
				lChild.ReplaceAndRemoveMin(targetTNode, fio, left);
				// we need to set the Height and balance after removal
				SetHeightAndBalance(fio, PositionInParent);
			}
			// if there is not left child, swap the data
			else {
				// swapping the data
				targetTNode.data = data;
				// attach PositionInParent to the right child
				PositionInParent = right;
			}
		}
}

/**
 * Name: Remove
 * Purpose and description:
 * This method removes an element into the tree. Return true or false indicating
 * success of removal.
 * 
 * @param elementTNode TNode<Whatever> type -> reference to the item to be 
 *		removed 
 * @param fio fstream * type -> filestream to the datafile on disk
 * @param occupancy -> reference to the occuapcny of the tree
 * @param PositionInParent TNode<Whatever> type -> reference to Parent's
 *		pointer
 * @param fromSHB bool type -> whether Remove was called from SHAB
 * @return unsigned long type -> 1 for success, 0 for failure
 */
template <class Whatever>
unsigned long TNode<Whatever> :: Remove (TNode<Whatever> & elementTNode,
	fstream * fio, long & occupancy, offset & PositionInParent,
	long fromSHB) {
	unsigned long returnVal = TRUE; // storing return value
	bool deleted = false; // if TNode is deleted

	// when we find the matching data
	if (data == elementTNode.data){
		elementTNode.data = data; // swap the data
		deleted = true; // turn the flag on
		// no children case
		if (!left && !right){
			PositionInParent = 0;
			returnVal = TRUE;
		}
		// only right child case
		else if (!left){
			PositionInParent = right; // re-attach it to the right
			returnVal = TRUE;
		}
		// only left child case
		else if (!right){
			PositionInParent = left; // re-attach it the left
			returnVal = TRUE;
		}
		// two children case
		else {
			TNode<Whatever> rChild (right, fio);
			rChild.ReplaceAndRemoveMin(*this, fio, right); // right child call
			deleted = false; // set flag to false
			// write it to disk only if it is called from SHB
			// otherwise SHAB will write it to the disk
			if (fromSHB)
				Write(fio);
		}
		// decrement occupancy
		occupancy--;
	}
	else if (elementTNode.data < data){
		// cannot locate the element
		if (!left){
			return FALSE;
		}
		// recursively calling to the left
		TNode<Whatever> lChild (left, fio);
		returnVal = lChild.Remove(elementTNode, fio, occupancy, left, fromSHB);
	}
	else{
		// cannot locate the element
		if (!right){
			return FALSE;
		}
		// recursively calling to the right
		TNode<Whatever> rChild (right, fio);
		returnVal = rChild.Remove(elementTNode, fio, occupancy, right, fromSHB);
	}
	if (!fromSHB && !deleted){
		// calling SHB if not called from SHB
		SetHeightAndBalance(fio, PositionInParent); 
	}
	return returnVal;
}

/**
 * Name: Tree<Whatever> :: Remove (public) 
 * Purpose and description:
 * This method delegates to TNode remove to remove the data
 *
 * @param   element   Whatever type -> the element to te inserted 
 * @return  unsigned long type -> 1 for success and 0 for failure
 */
template <class Whatever>
unsigned long Tree<Whatever> :: Remove (Whatever & element) {
	IncrementOperation();
	long returnVal = FALSE; // store return value
	// if the tree is null, return FALSE
	if (!occupancy){
		return returnVal;
	}
	// create a TNode object on RTS
	TNode<Whatever> working (element);
	// read in the rootNode
	TNode<Whatever> rootNode (root, fio);
	// delegates to TNode's remove
	returnVal = rootNode.Remove(working, fio, occupancy, root, FALSE);
	// swap the data
	element = working.data;

	// reset the root when the last TNode is removed
	if (!occupancy){
		ResetRoot(); 
	}

	return returnVal;
}

/**
 * Name: SetHeightAndBalance 
 * Purpose and description:
 * This method sets the height and balance of the node.
 * 
 * @param fio fstream * type -> filestream to the datafile on disk
 * @param PositionInParent offset type -> reference to parent's position
 */
template <class Whatever>
void TNode<Whatever> :: SetHeightAndBalance (fstream * fio,
		offset & PositionInParent) {
	long leftHeight = -1; // left child's height
	long rightHeight = -1; // right child's height
	// update left child's height if there's one
	if (left){
		TNode<Whatever> lChild (left, fio);
		leftHeight = lChild.height;
	}
	// update right child's height if there's one
	if (right){
		TNode<Whatever> rChild (right, fio);
		rightHeight = rChild.height;
	}
	// update the height of the current TNode
	if (leftHeight > rightHeight){
		height = leftHeight + 1;
	}
	else {
		height = rightHeight + 1;
	}
	balance = leftHeight - rightHeight; // update balance of current TNode

	// keep the balance of the tree
	if (abs(balance) > THRESHOLD){
		long temp_occupancy = 0;
		TNode<Whatever> temp2 (data);
		Whatever element = data;
		// Remove the off balance TNode
		Remove(temp2, fio, temp_occupancy, PositionInParent, TRUE);
		// get the temp Node to call Insert
		TNode<Whatever> temp (PositionInParent, fio);
		temp.Insert(element, fio, temp_occupancy, PositionInParent);
	}
	// write the node to disk if not out of balance
	else{
		Write(fio);
	}
}

/**
 * Name: Tree<Whatever> :: GetCost (public)
 * Purpose and description:
 * This method returns the value of the Tree<Whatever> :: cost variable
 * 
 * @return long type -> cost of the tree
 */
template <class Whatever>
long Tree <Whatever> :: GetCost () {
	return cost;
}

/**
 * Name: Tree<Whatever> :: GetCost (public)
 * Purpose and description:
 * This method returns the value of the Tree<Whatever>::operation variable.
 *
 * @return long type -> opeartion of the tree
 */
template <class Whatever>
long Tree <Whatever> :: GetOperation () {
	return operation;
}

/**
 * Name: Tree<Whatever> :: IncrementCost (public)
 * Purpose and description:
 * Increments the value of the Tree<Whatever>::cost varibale. This should be 
 * called when a read or write to disk occurs.
 *
 */
template <class Whatever>
void Tree <Whatever> :: IncrementCost () {
	cost++;
}

/**
 * Name: Tree<Whatever> :: IncrementOperation (public)
 * Purpose and description:
 * Increments the value of the Tree<Whatever>::operation varibale. This should 
 * be called when a read or write to disk occurs.
 *
 */
template <class Whatever>
void Tree <Whatever> :: IncrementOperation () {
	operation++;
}

/**
 * Name: Tree<Whatever> :: ResetRoot (public)
 * Purpose and description:
 * Resets the root datafield of this tree to be at the end of the datafile. 
 * This should be called when the last TNode has been removed from the Tree.
 *
 */
template <class Whatever>
void Tree <Whatever> :: ResetRoot () {
	fio -> seekg(0, ios::end);
	root = fio -> tellg();
}

/**
 * Name: TNode<Whatever> :: Insert 
 * Performs insertion into the binary search tree. It will return true if the
 * insertion is successful, and false if the insertion is unseccessful.
 *
 * @param  element   Whatever type -> the element to te inserted 
 * @param	 fio fstream * type -> filestream to the datafile on disk
 * @param  occupancy -> reference to the occupancy of the tree
 * @param  PositionInParent TNode<Whatever> -> refernece to parent's position
 * @return long type -> TRUE for success, FALSE for unsuccess  
 */
template <class Whatever>
unsigned long TNode<Whatever> :: Insert (Whatever & element, fstream * fio,
		long & occupancy, offset & PositionInParent) {
	// when we find the matching element
	if (data == element){
		// swap the data
		data = element;
		// write the swapped Node to disk
		Write(fio);
		return TRUE; 
	}
	// go left if the element is smaller
	else if (element < data){
		// keeping going left if there is left child
		if (left){
			// read left child
			TNode<Whatever> lChild (left, fio);
			lChild.Insert(element, fio, occupancy, left);
		}
		// insert it when there is no left child
		else {
			TNode<Whatever> newNode (element, fio, occupancy);
			// assign the newNode position to left data field
			left = newNode.this_position;
		}
	}
	// go right if the element is smaller
	else {
		// keeping going right if there is right child
		if (right){
			// read right child
			TNode<Whatever> rChild (right, fio);
			rChild.Insert(element, fio, occupancy, right);
		}
		// insert it when there is no right child
		else {
			TNode<Whatever> newNode (element, fio, occupancy);
			// assign the newNode position to right data field
			right = newNode.this_position;
		}
	}
	SetHeightAndBalance(fio, PositionInParent);
	return TRUE;
}

/**
 * Name: Tree<Whatever> :: Lookup (public) 
 * Purpose and description:
 * This method looks up the matching data in the binary tree. It returns one 
 * for success and 0 for failure.
 *
 * @param   element   Whatever type -> the element to te inserted 
 * @return  unsigned long type -> 1 for success and 0 for failure
 */
template <class Whatever>
unsigned long Tree<Whatever> :: Lookup (Whatever & element) const {
	IncrementOperation(); // increment operation
	long returnVal = FALSE; // store return value
	// if empty tree, return FALSE
	if (!occupancy){
		return returnVal;
	}
	// get the rootNode and delegate to TNode's Lookup
	TNode<Whatever> rootNode (root, fio);
	returnVal = rootNode.Lookup(element, fio);
	return returnVal;
}

/**
 * Name: Tree<Whatever> :: Read (public) 
 * Purpose and description:
 * Reads a TNode which is present on the datafile into the memory. The TNode's
 * information in the datafile overwrites this TNode's data. 
 *
 * @param   position offset type -> offset in the datafile corresponding to the 
 *		position of the TNode we wish to read into memory
 * @param 	fio fstream * type -> filestream to the datafile stored on disk
 * 
 */
template <class Whatever>
void TNode<Whatever> :: Read (const offset & position, fstream * fio) {
	fio -> seekg(position); // go to the position in file
	fio -> read((char *) this, sizeof(TNode<Whatever>)); // read the node
	// debug statement
	if (Tree<Whatever>::debug_on){
		cerr<<COST_READ<<(const char *) this <<RIGHT_BRACKET;
	}
	Tree<Whatever>::IncrementCost(); // increment cost
}

/**
 * Name: TNode 
 * Purpose and description:
 * This method is called when reading a TNode present on disk into memoryG
 * 
 * @param   position offset type -> offset in the datafile corresponding to the 
 *	position of the TNode we wish to read into memory
 * @param 	fio fstream * type -> filestream to the datafile stored on disk
 * @return  TNode<Whatever> ; 
 */
template <class Whatever>
TNode<Whatever> :: TNode (const offset & position, fstream * fio) {
	Read(position, fio); // delegate to Read method
}

/**
 * Name: TNode 
 * Purpose and description:
 * This method is called when creating TNode for the first time
 * 
 * @param   element   Whatever type -> the element to te inserted 
 * @param 	fio fstream * type -> filestream to the datafile stored on disk
 * @param   occupancy -> reference to the occupancy of the tree
 * @return  TNode<Whatever> ; 
 */
template <class Whatever>
TNode<Whatever> :: TNode (Whatever & element, fstream * fio, long & occupancy): 
	data (element), height (0), balance (0), left (0), 
	right (0) {
		occupancy++; // increase occupancy
		fio -> seekp(0, ios::end); // get to the end of file to write
		this_position = fio -> tellp(); // store the position
		Write(fio); // Write the TNode
	}

/**
 * Name: TNode 
 * Purpose and description:
 * This method writes this TNode object to disk at this_position in the datafile
 * 
 * @param 	fio fstream * type -> filestream to the datafile stored on disk
 */
template <class Whatever>
void TNode<Whatever> :: Write (fstream * fio) const {
	fio -> seekp(this_position); // seek to the position
	fio -> write((const char *) this, sizeof(TNode<Whatever>)); 
	// debug statement
	if (Tree<Whatever>::debug_on){
		cerr<<COST_WRITE<<(const char *) this<<RIGHT_BRACKET;
	}
	Tree<Whatever>::IncrementCost(); // increment cost
}

/**
 * Name: Tree<Whatever> :: Tree (public)
 * Purpose and description:
 * This constructor allocates the tree object. It checks the datafile to see if
 * it contains Tree data. If it is empty, root and occupancy fields are written
 * to the file. If there is data in the datafile, root and occupancy fields are
 * read into memory. 
 * 
 * @param 	fio fstream * type -> filestream to the datafile stored on disk
 */
template <class Whatever>
Tree<Whatever> :: Tree (const char * datafile) :
	fio (new fstream (datafile, ios :: out | ios :: in)), occupancy(0), root(0){

		tree_count = ++counter; // increment the tree_count by one

		// debug statement
		if (debug_on){
			cerr<<TREE<<tree_count<<ALLOCATE;
		}

		// get to the beginning and get the start position
		fio -> seekp(0, ios::beg);
		offset begin = fio -> tellp();

		// get to the end and get the end position
		fio -> seekp(0, ios::end);
		offset ending = fio -> tellp();

		// when start position equals end position, it's empty file
		if (begin == ending){
			// go the beginning and intialize root and occupancy data
			fio->flush();
			fio -> seekp(0, ios::beg);
			fio -> write((const char *) & root, sizeof(offset));
			fio -> write((const char *) & occupancy, sizeof(long)); 
			root = fio -> tellp(); // get the position of root data
		}
		else{
			// read in the root and occupancy data
			fio -> seekg(0, ios::beg);
			fio -> read((char *) & root, sizeof(offset));
			fio -> read((char *) & occupancy, sizeof(long));
		}
	}

/**
 * Name: Tree<Whatever> :: ~Tree (public)
 * Purpose and description:
 * It deallocates memory associated with the Tree. It will also delete all the
 * memory of the elements within the table. 
 *
 */
	template <class Whatever>
Tree<Whatever> :: ~Tree (void){
	// debug statement
	if (debug_on){
		cerr<<TREE<<tree_count<<DEALLOCATE;
	}
	// go to the beginning and write the root and occupancy
	fio -> seekp(0, ios::beg);
	// write the root and occupancy to disk
	fio -> write((const char *) & root, sizeof(offset));
	fio -> write((const char *) & occupancy, sizeof(long));
	delete fio; // delete the fio pointer
}	

template <class Whatever>
ostream & operator << (ostream & stream, const TNode<Whatever> & nnn) {
	stream << "at height:  :" << nnn.height << " with balance:  "
		<< nnn.balance << "  ";
	return stream << nnn.data << "\n";
}

/**
 * Name: Tree<Whatever> :: Write (public)
 * Purpose and description:
 * This function will output the contents of the Tree table to the stream
 * specified by the caller. The stream could be cerr, cout, or any other valid
 * stream.
 *
 * @param   stream -> A reference to the output stream 
 * @return  <return> -> A reference to the output stream 
 */
template <class Whatever>
ostream & Tree<Whatever> :: Write (ostream & stream) const
{
	long old_cost = cost;

	stream << "Tree " << tree_count << ":\n"
		<< "occupancy is " << occupancy << " elements.\n";

	fio->seekg (0, ios :: end);
	offset end = fio->tellg ();

	// check for new file
	if (root != end) {
		TNode<Whatever> readRootNode (root, fio);
		readRootNode.Write_AllTNodes (stream, fio);
	}

	// ignore cost when displaying nodes to users
	cost = old_cost;

	return stream;
}

/**
 * Name: Tree<Whatever> :: Set_Debug_On (public)
 * Purpose and description:
 * This method sets the debug to true.
 *
 */
template <class Whatever>
void Tree<Whatever> :: Set_Debug_On (void)
{
	debug_on = true;
}

/**
 * Name: Tree<Whatever> :: Set_Debug_Off (public)
 * Purpose and description:
 * This method sets the debug to false.
 *
 */
template <class Whatever>
void Tree<Whatever> :: Set_Debug_Off (void)
{
	debug_on = false;
}

template <class Whatever>
ostream & TNode<Whatever> ::
Write_AllTNodes (ostream & stream, fstream * fio) const {
	if (left) {
		TNode<Whatever> readLeftNode (left, fio);
		readLeftNode.Write_AllTNodes (stream, fio);
	}
	stream << *this;
	if (right) {
		TNode<Whatever> readRightNode (right, fio);
		readRightNode.Write_AllTNodes (stream, fio);
	}

	return stream;
}

