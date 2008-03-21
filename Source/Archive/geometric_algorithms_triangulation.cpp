#include "geometric_algorithms.h"


/***********************************************~***************************************************/


#define TRIANGULATIONTYPE_REGULAR				0
#define TRIANGULATIONTYPE_START					1
#define TRIANGULATIONTYPE_END					2
#define TRIANGULATIONTYPE_SPLIT					3
#define TRIANGULATIONTYPE_MERGE					4


struct TriangulateEdge;
struct BSTNode;

struct TriangulateVertex {
	int id;
	int type;
	WCVector4 vertex;
	WPFloat internalAngle;
	TriangulateVertex *left, *right;
	TriangulateEdge *edge;
};


struct TriangulateEdge {
	BSTNode *node;
	TriangulateVertex *helper;
	TriangulateVertex *start, *end;
	TriangulateEdge *cw, *ccw;
};


struct BSTNode {
	TriangulateVertex *vertex;
	TriangulateEdge *edge;
	BSTNode *left, *right;
};


//struct TriangulateEdgeUse {
//	TriangulateEdge *edge;
//	TriangulateEdgeUse *cw, *ccw;
//};


/***********************************************~***************************************************/


void TriangulateSortFlatten(BSTNode *node, std::list<TriangulateVertex*> &list) {
	//Add left children (if any)
	if (node->left != NULL) TriangulateSortFlatten(node->left, list);
	//Add this node
	list.push_back(node->vertex);
	//Add right children (if any)
	if (node->right != NULL) TriangulateSortFlatten(node->right, list);
	//Delete the node
	delete node;
}


void TriangulateSortPlace(BSTNode *refNode, BSTNode *node) {
	//Check same y value case
	if (node->vertex->vertex.J() == refNode->vertex->vertex.J()) {
		//See which one has lesser x value
		if (node->vertex->vertex.I() < refNode->vertex->vertex.I()) {
			if (refNode->left == NULL) refNode->left = node;
			else TriangulateSortPlace(refNode->left, node);
		}
		else {
			if (refNode->right == NULL) refNode->right = node;
			else TriangulateSortPlace(refNode->right, node);
		}
		return;
	}
	//Check left child
	if (node->vertex->vertex.J() > refNode->vertex->vertex.J()) {
		//Check to see if there is a left-child
		if (refNode->left == NULL) refNode->left = node;
		//If there is, recurse down the tree
		else TriangulateSortPlace(refNode->left, node);
		return;
	}
	//Must be right child
	if (refNode->right == NULL) refNode->right = node;
	else TriangulateSortPlace(refNode->right, node);
}



/*** Using a modified version of heap sort.  First is vertex with max y (use min x, if needed) ***/
void TriangulateSort(std::list<TriangulateVertex*> &vertexList) {
	std::list<TriangulateVertex*>::iterator vertIter = vertexList.begin();
	BSTNode *root = new BSTNode(), *node;
	root->vertex = *vertIter;
	root->left = NULL;
	root->right = NULL;
	//Loop through all remaining vertices and build heap
	for (vertIter++; vertIter != vertexList.end(); vertIter++) {
		node = new BSTNode();
		node->vertex = *vertIter;
		node->left = NULL;
		node->right = NULL;
		TriangulateSortPlace(root, node);
	}
	//Clear vertexList and flatten heap
	vertexList.clear();
	TriangulateSortFlatten(root, vertexList);
}


/***********************************************~***************************************************/


WPFloat TriangulateInternalAngle(WCVector4 &pt, WCVector4 &left, WCVector4 &right) {
	WCVector4 v0 = left - pt;
	WCVector4 v1 = right - pt;
	WCVector4 cross = v1.CrossProduct(v0);
	//Determine the angle
	WPFloat retVal = acos( v0.DotProduct(v1) / (v0.Magnitude() * v1.Magnitude()) );
	//If cross.K is negative, invert the angle
	if (cross.K() < 0.0) retVal =  2.0 * M_PI - retVal;
	return retVal;
}


void TriangulateDetermineType(TriangulateVertex *vertex) {
	bool leftAbove=false, rightAbove=false;
	//Is left at above or at same and x is greater, mark left as above
	if ( (vertex->left->vertex.J() > vertex->vertex.J()) || 
		((vertex->left->vertex.J() == vertex->vertex.J()) && (vertex->left->vertex.I() < vertex->vertex.I()))) leftAbove = true;

	//Is right at above or at same and x is greater, mark left as above
	if ( (vertex->right->vertex.J() > vertex->vertex.J()) || 
		((vertex->right->vertex.J() == vertex->vertex.J()) && (vertex->right->vertex.I() < vertex->vertex.I()))) rightAbove = true;

	//Calculate internal angle
	WPFloat internalAngle = TriangulateInternalAngle(vertex->vertex, vertex->left->vertex, vertex->right->vertex);
	
	//Start Case
	if (!leftAbove && !rightAbove) {
		if (internalAngle < M_PI) vertex->type = TRIANGULATIONTYPE_START;
		else vertex->type = TRIANGULATIONTYPE_SPLIT;
	}
	else if (leftAbove && rightAbove) {
		if (internalAngle < M_PI) vertex->type = TRIANGULATIONTYPE_END;
		else vertex->type = TRIANGULATIONTYPE_MERGE;
	}
	else vertex->type = TRIANGULATIONTYPE_REGULAR;
}


/***********************************************~***************************************************/


bool BSTIsLeftOf(TriangulateEdge *edge, TriangulateEdge *refEdge) {
	WCVector4 start, end, point;
	
	//Make sure edge->start is not same as refEdge start or end
	if ((edge->start->vertex.Distance(refEdge->start->vertex) > 0.0001) && 
		(edge->start->vertex.Distance(refEdge->end->vertex) > 0.0001)) point = edge->start->vertex;
	//Otherwise, use edge->end
	else point = edge->end->vertex;

	//If ref horizontal case
	if (refEdge->start->vertex.J() == refEdge->end->vertex.J()) {
		//See if start of refEdge is left of end
		if (refEdge->start->vertex.I() < refEdge->end->vertex.I()) {
			start = refEdge->start->vertex;
			end = refEdge->end->vertex;			
		}
		//End is left of start
		else {
			start = refEdge->end->vertex;
			end = refEdge->start->vertex;
		}
		return false;
	}
	//Ref pointing up case
	else if (refEdge->end->vertex.J() > refEdge->start->vertex.J()) {
		//Use refEdge start and end, and edge->start
		start = refEdge->start->vertex;
		end = refEdge->end->vertex;
	}
	//Ref pointing down case
	else {
		start = refEdge->end->vertex;
		end = refEdge->start->vertex;
	}
	//Test for lefty-ness
	if ( (end.I() - start.I()) * (start.J() - point.J()) - (start.I() - point.I()) * (end.J() - start.J()) < 0.0) return true;
	return false;
}


TriangulateEdge* BSTFindNextLeft(TriangulateVertex *vertex, BSTNode *rootNode) {
	BSTNode *lastRight = rootNode, *curNode = rootNode;
	//Keep looping
	do {
		//Is vertex left of curNode
		if (BSTIsLeftOf(vertex->edge, curNode->edge)) 
			curNode = curNode->left;
		else {
			lastRight = curNode;
			curNode = curNode->right;
		}	
	}
	//Unitl curNode has no children
	while (curNode != NULL);
	//Closest left edge is where we last made a right
	return lastRight->edge;
}


void BSTInsertNode(BSTNode *node, BSTNode *&refNode) {
	//See if this is the new root
	if (refNode == NULL) {
		refNode = node;
		return;
	}
	//Check to see if left of refNode
	if ( BSTIsLeftOf(node->edge, refNode->edge) ) {
		//If no left child, add to tree
		if (refNode->left == NULL) refNode->left = node;
		//Otherwise, recursively call left child
		else BSTInsertNode(node, refNode->left);
	}
	//Go down right-hand path
	else {
		//If no right child, add to tree
		if (refNode->right == NULL) refNode->right = node;
		//Otherwise, recursively call right child
		else BSTInsertNode(node, refNode->right);	
	}
}


void BSTCascadeDelete(BSTNode *node) {
	//Delete left children
	if (node->left != NULL) BSTCascadeDelete(node->left);
	//Delete right children
	if (node->right != NULL) BSTCascadeDelete(node->right);
	//Delete node
	delete node;
}


void BSTDeleteNode(BSTNode *node, BSTNode *&refNode) {
	//See if we are deleting root node
	if (node == refNode) {
		BSTCascadeDelete(node);
		refNode = NULL;
		return;
	}
	//See if refNode is parent of node
	if ((refNode->left == node) || (refNode->right == node)) {
		//Delete node and all its children
		BSTCascadeDelete(node);
		//Set appropriate child pointer to null
		if (refNode->left == node) refNode->left = NULL;
		else refNode->right == NULL;
	}
	//Otherwise pass to children
	BSTDeleteNode(node, refNode->left);
	BSTDeleteNode(node, refNode->right);
}


void BSTPrint(BSTNode* node) {
	//Check for null case
	if (node == NULL) { std::cout << "-" << std::endl; return; }
	//Print left children
	if (node->left != NULL) BSTPrint(node->left);
	//Print self
	std::cout << "Edge: " << node->edge->start->id << std::endl;
	//Print right children
	if (node->right != NULL) BSTPrint(node->right);
}


/***********************************************~***************************************************/


bool TriangulateInteriorOnRight(TriangulateVertex *vertex) {
	//Check equal case
	if (vertex->left->vertex.J() == vertex->right->vertex.J()) {
		if (vertex->left->vertex.I() < vertex->right->vertex.I()) return true;
		else return false;
	}
	//See if left is higher
	if (vertex->left->vertex.J() > vertex->right->vertex.J()) return true;
	else return false;
}


void TriangulateHandleRegular(TriangulateVertex *vertex, BSTNode *&root) {
	//If the interior lies to the right of v_i
	if (TriangulateInteriorOnRight(vertex)) {
		//If helper(e_i->cw) is a merge vertex
		if (vertex->edge->cw->helper->type == TRIANGULATIONTYPE_MERGE) {
			//Insert diagonal connecting v_i to helper(e_i->cw) into D
			//...
		}
		//Delete e_i->cw from T
		BSTDeleteNode(vertex->edge->cw->node, root);
		//Set helper(e_i) = v_i
		vertex->edge->helper = vertex;
		//Create a new BSTNode
		BSTNode *node = new BSTNode();
		node->edge = vertex->edge;
		node->edge->node = node;
		node->left = NULL;
		node->right = NULL;
		//Insert e_i into T
		BSTInsertNode(node, root);
	}
	//If interior it to the left
	else {
		//Search in tree for edge e_j directly left of vertex
		TriangulateEdge *left = BSTFindNextLeft(vertex, root);
		//If helper(e_j) is a merge vertex
		if (left->helper->type == TRIANGULATIONTYPE_MERGE) {
			//Insert diagonal connecting v_i to helper(e_j) into D
			//...
		}
		//Set helper(e_j) = v_i
		left->helper = vertex;
	}
}


void TriangulateHandleStart(TriangulateVertex *vertex, BSTNode *&root) {
	//Set helper(e_i) to v_i
	vertex->edge->helper = vertex;
	//Create a new BSTNode
	BSTNode *node = new BSTNode();
	node->edge = vertex->edge;
	node->edge->node = node;
	node->left = NULL;
	node->right = NULL;
	//Insert e_i into T
	BSTInsertNode(node, root);
}


void TriangulateHandleEnd(TriangulateVertex *vertex, BSTNode *&root) {
	//If helper(e_i->cw) is a merge vertex
	if (vertex->edge->cw->helper->type == TRIANGULATIONTYPE_MERGE) {
		//Insert diagonal connecting v_i to helper(e_i->cw) into D
		//...
	}
	//Delete e_i->cw from T
	BSTDeleteNode(vertex->edge->cw->node, root);
}


void TriangulateHandleSplit(TriangulateVertex *vertex, BSTNode *&root) {
	//Search in tree for edge e_j directly left of vertex
	TriangulateEdge *left = BSTFindNextLeft(vertex, root);
	//Insert diagonal connecting v_i to helper(e_j) in D
	//...
	//Set helper(e_j) = v_i
	left->helper = vertex;
	//Set helper(e_i) = v_j
	vertex->edge->helper = vertex;
	//Create a new BSTNode
	BSTNode *node = new BSTNode();
	node->edge = vertex->edge;
	node->edge->node = node;
	node->left = NULL;
	node->right = NULL;
	//Insert e_i into T
	BSTInsertNode(node, root);
}


void TriangulateHandleMerge(TriangulateVertex *vertex, BSTNode *&root) {
	//If helper(e_i->cw) is MERGE
	if (vertex->edge->cw->helper->type == TRIANGULATIONTYPE_MERGE) {
		//Insert diagonal connecting v_i to helper(e_i->cw) into D
		//...
	}
	//Delete e_i->cw from T
	BSTDeleteNode(vertex->edge->cw->node, root);
	//Search in tree for edge e_j directly left of v_i
	TriangulateEdge *left = BSTFindNextLeft(vertex, root);
	//If helper(e_j) is merge vertex
	if (left->helper->type == TRIANGULATIONTYPE_MERGE) {
		//Insert diagonal connecting v_i to helper(e_j) into D
		//...
	}
	//Set helper(e_j) = v_i
	left->helper = vertex;
}


/***********************************************~***************************************************/


GLint* TriangulatePolygon(const std::list<WCVector4> &pointList) {
	//Create vertex structs for each point
	std::list<WCVector4>::const_iterator pointIter = pointList.begin();
	std::list<TriangulateVertex*> priorityQueue;
	int id = 0;
	TriangulateVertex *vert, *rightVert = NULL;
	TriangulateEdge *edge;
	//Process remaining vertices
	for (pointIter = pointList.begin(); pointIter != pointList.end(); pointIter++) {
		//Create the new vertex
		vert = new TriangulateVertex();
		//See about cleaning up last vert
		if (rightVert != NULL) rightVert->left = vert;
		vert->id = id++;
		vert->vertex = *pointIter;
		vert->right = rightVert;
		//Setup edge and add to vector
		edge = new TriangulateEdge();
		vert->edge = edge;
		//Add vertex to list and vector
		priorityQueue.push_back(vert);
		//Swap vert into rightVert
		rightVert = vert;
	}
	//Clean up first and last verts
	priorityQueue.front()->right = priorityQueue.back();
	priorityQueue.back()->left = priorityQueue.front();
	
	std::list<TriangulateVertex*>::iterator vertIter;
	//Finish setting up edges
	for (vertIter = priorityQueue.begin(); vertIter != priorityQueue.end(); vertIter++) {
		//Set all of the edge properties
		edge = (*vertIter)->edge;
		edge->helper = NULL;
		edge->start = (*vertIter);
		edge->end = (*vertIter)->left;
		edge->cw = edge->end->edge;
		edge->ccw = edge->start->right->edge;
		//Show some info
		std::cout << "Edge(" << edge << ") -  S:" << edge->start->id << ", E:" << edge->end->id << std::endl;
		std::cout << "\tHelper: " << edge->helper << ", CW: " << edge->cw << ", CCW: " << edge->ccw << std::endl;
	}
	
	//Sort the vertices
	TriangulateSort(priorityQueue);
	
	
	//Determine the type of each vertex
	for (vertIter = priorityQueue.begin(); vertIter != priorityQueue.end(); vertIter++) {
		TriangulateDetermineType( *vertIter );
		std::cout << "Vert(" << *vertIter << "): " << (*vertIter)->id << ", " << (*vertIter)->type << " :: " << (*vertIter)->vertex << std::endl;
		std::cout << "\tEdge: " << (*vertIter)->edge << ", Left: " << (*vertIter)->left << ", Right: " << (*vertIter)->right << std::endl;
	}
	
	//Process first vertex and create Binary Search Tree
	BSTNode *bstRoot = NULL;
	BSTPrint(bstRoot);
	//Loop until all vertices have been handled
	while (priorityQueue.size() != 0) {
		//Get the next vertex
		vert = priorityQueue.front();
		priorityQueue.pop_front();
		//Handle the vertex
		switch(vert->type) {
			case TRIANGULATIONTYPE_REGULAR: TriangulateHandleRegular(vert, bstRoot); break;
			case TRIANGULATIONTYPE_START: TriangulateHandleStart(vert, bstRoot); break;
			case TRIANGULATIONTYPE_END: TriangulateHandleEnd(vert, bstRoot); break;
			case TRIANGULATIONTYPE_SPLIT: TriangulateHandleSplit(vert, bstRoot); break;
			case TRIANGULATIONTYPE_MERGE: TriangulateHandleMerge(vert, bstRoot); break;
		}
		//Print the tree (left-to-right)
		BSTPrint(bstRoot);
	}
	
	//Now we have monotone polynomials in the edges, so triangulate
	
	//...
	
	exit(0);
	//Delete edge lists
	//...
	return NULL;
}


/***********************************************~***************************************************/

