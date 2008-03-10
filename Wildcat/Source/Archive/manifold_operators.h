#ifndef __WTPKL_MANIFOLD_OPERATORS_H
#define __WTPKL_MANIFOLD_OPERATORS_H


/*** Included Header Files ***/
#include "euler_operators.h"


/*** Locally Defined Values ***/
//None


/*** Class Order ***/
//CManifold


/*****************************************************************************/


class CManifold {
public:
	/* Creates the needed uses and associations for the new model, region, shell, face, loop, and vertex */
	static void MakeModel(CModel *newModel, CRegion *newRegion, CShell *newShell, CFace *newFace, CLoop *newLoop, CVertex *newVertex);
	/* Kills the model completely (you still have to delete all of the geometry parts (model, region, shell, face, etc.) */
	static void KillModel(CModel *model);

	
	/* Creates a newEdge and newVertex connecting to vertex on face.  If no reference edge is passed, the new edge will
	 * be clockwise of the last outgoing EdgeUse on the Face from the Vertex.  If a reference edge is passed, the new edge
	 * will be clockwise of that edge (as long as it is on face). */
	static void MakeEdgeVertex(CVertex *vertex, CFace *face, CEdge *newEdge, CVertex *newVertex, CEdge* edge=NULL);	
	/* Kills the passed edge and vertex from the given face */
	static void KillEdgeVertex(CVertex *vertex, CEdge *edge, CFace *face) {}
	
	
	/* Creates a new edge between vertex 1 and vertex 2.  It also creates a new face and loop.  If v1 and v2 are the same
	 * vertex, as of now nothing happens. */
	static void MakeEdgeFace(CVertex *v1, CVertex *v2, CFace *face, CEdge *newEdge, CLoop *newLoop, CFace *newFace);
	/* XXX */
	static void KillEdgeFaceLoop(CEdge *edge, CFace *face) {}
	 
	 
	/* XXX */
	static void MakeEdgeKillLoop(CVertex *v1, CVertex *v2, CFace *face, CEdge *newEdge) {}
	/* Kill the given edge (must be used twice within the loop, and only those two times) and place all items on the Vertex
	 * side of the edge into the new face */ 
	static void KillEdgeMakeLoop(CEdge *edge, CVertex *vertex, CLoop *newLoop); 
	
	/* XXX */
	static void MakeEdgeKillShellFaceLoop(void) {}
	/* XXX */
	static void KillEdgeMakeShellFaceLoop(void) {}
	
	/* XXX */
	static void KillFaceLoopEdgeVertexMakeHole(void) {}
	/* Remove the face from a given shell this causes a new hole to be created in the model */
	static void KillFaceMakeLoopHole(CFace *oldFace, CFace *otherFace, CLoop *newLoop);
	
	
	static void KillFaceLoopEdgeVertexShell(void) {}
	static void KillShellFaceLoopEdgeVertex(void) {}
	static void EdgeSqueeze(void) {}
	static void EdgeSplit(void) {}
	static void MakeFaceLoopEdgeVertexKillHole(void) {}
	static void MakeFaceLoopEdgeVertexShell(void) {}
	static void KillVertexEdges(void) {}
	static void LMove(void) {}
	//Boolean operators
};


/*****************************************************************************/


#endif //__WTPKL_MANIFOLD_OPERATORS_H

