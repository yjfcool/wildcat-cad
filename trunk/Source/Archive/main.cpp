#include "logger.h"
#include "constraint_tag.h"
#include "constraint_cluster.h"
#include "constraint_planner.h"
//#include "bipartite_flow_network.h"

void TwinBasicTriangles(CConstraintPlanner &cPlanner) {
	//Constraint Objects
	CPointTag2D* pA = new CPointTag2D(0.0, 0.0);
	CPointTag2D* pB = new CPointTag2D(10.0, 0.0);
	CPointTag2D* pC = new CPointTag2D(10.0, 10.0);
	CPointTag2D* pD = new CPointTag2D(10.0, 10.0);
	CPointTag2D* pE = new CPointTag2D(10.0, 10.0);
	CPointTag2D* pF = new CPointTag2D(10.0, 10.0);
	
	//Constraints
	CConstraintTag2D* dcAB = CConstraintTag2D::Distance(10.0);
	CConstraintTag2D* dcAC = CConstraintTag2D::Distance(10.0);
	CConstraintTag2D* dcBC = CConstraintTag2D::Distance(10.0);
	CConstraintTag2D* dcDE = CConstraintTag2D::Distance(10.0);
	CConstraintTag2D* dcDF = CConstraintTag2D::Distance(10.0);
	CConstraintTag2D* dcEF = CConstraintTag2D::Distance(10.0);
	
	CConstraintNode *nA, *nB, *nC, *nD, *nE, *nF;
	
	//Set up the primary constraint planner
	nA = cPlanner.AddObject(pA);
	nB = cPlanner.AddObject(pB);
	nC = cPlanner.AddObject(pC);
	nD = cPlanner.AddObject(pD);
	nE = cPlanner.AddObject(pE);
	nF = cPlanner.AddObject(pF);
	
	//Constraints between the objects
	cPlanner.AddConstraint(nA, nB, dcAB);
	cPlanner.AddConstraint(nA, nC, dcAC);
	cPlanner.AddConstraint(nB, nC, dcBC);
	cPlanner.AddConstraint(nD, nE, dcDE);
	cPlanner.AddConstraint(nD, nF, dcDF);
	cPlanner.AddConstraint(nE, nF, dcEF);

}


void FixedTriangle(CConstraintPlanner &cPlanner) {
	//Constraint Objects
	CPointTag2D* pA = new CPointTag2D(0.0, 0.0);
	CPointTag2D* pB = new CPointTag2D(10.0, 0.0);
	CPointTag2D* pC = new CPointTag2D(10.0, 10.0);
	CLineTag2D* xLine = new CLineTag2D(0.0, 0.0, 0.0);
	CLineTag2D* yLine = new CLineTag2D(1.0, 1.0, 1.0);
	
	//Constraints
	CConstraintTag2D* dcAB = CConstraintTag2D::Distance(10.0);
	CConstraintTag2D* dcAC = CConstraintTag2D::Distance(10.0);
	CConstraintTag2D* dcBC = CConstraintTag2D::Distance(10.0);
	CConstraintTag2D* dcAE = CConstraintTag2D::Distance(10.0);
	CConstraintTag2D* dcXA = CConstraintTag2D::Distance(10.0);
	CConstraintTag2D* dcXB = CConstraintTag2D::Distance(10.0);
	CConstraintTag2D* dcYA = CConstraintTag2D::Distance(10.0);

	CConstraintNode *nA, *nB, *nC, *xAxis, *yAxis;
	
	/*** Main Body ***/
	
	//Set up the primary constraint planner
	nA = cPlanner.AddObject(pA);
	nB = cPlanner.AddObject(pB);
	nC = cPlanner.AddObject(pC);
	xAxis = cPlanner.AddObject(xLine, 0);
	yAxis = cPlanner.AddObject(yLine, 0);
		
	//Constraints between the objects
	cPlanner.AddConstraint(nA, nB, dcAB);
	cPlanner.AddConstraint(nA, nC, dcAC);
	cPlanner.AddConstraint(nB, nC, dcBC);
	
	//Constraints to the reference coordinate system
	cPlanner.AddConstraint(nA, xAxis, dcXA);
	cPlanner.AddConstraint(nB, xAxis, dcXB);
	cPlanner.AddConstraint(nA, yAxis, dcYA);
}


void BasicTriangle(CConstraintPlanner &cPlanner) {
	//Constraint Objects
	CPointTag2D* pA = new CPointTag2D(0.0, 0.0);
	CPointTag2D* pB = new CPointTag2D(10.0, 0.0);
	CPointTag2D* pC = new CPointTag2D(10.0, 10.0);

	//Constraints
	CConstraintTag2D* dcAB = CConstraintTag2D::Distance(10.0);
	CConstraintTag2D* dcAC = CConstraintTag2D::Distance(10.0);
	CConstraintTag2D* dcBC = CConstraintTag2D::Distance(10.0);

	CConstraintNode *nA, *nB, *nC;

	//Set up the primary constraint planner
	nA = cPlanner.AddObject(pA);
	nB = cPlanner.AddObject(pB);
	nC = cPlanner.AddObject(pC);
	
	//Constraints between the objects
	cPlanner.AddConstraint(nA, nB, dcAB);
	cPlanner.AddConstraint(nA, nC, dcAC);
	cPlanner.AddConstraint(nB, nC, dcBC);
}


int main (int argc, char * const argv[]) {
	CConstraintPlanner cPlanner;
	CPlan* plan;
	
	//Initialize the global logger
	CLogger::Initialize( CLoggerLevel::Warn() );	
	
	//BasicTriangle(cPlanner);
	TwinBasicTriangles(cPlanner);
	//FixedTriangle(cPlanner);
	
	cPlanner.Print();
	plan = cPlanner.GetPlan( DConst::Float2D() );
	//plan = cPlanner.GetPlan( DConst::Fixed() );
	plan->Print();
	switch(plan->GetStatus()) {
		case -1: cout << "Solution under-constrained\n"; break;
		case 0: cout << "Solution found!!!!\n"; break;
		case 1: cout << "Solution over-constrained\n"; break;
	}
	
	//Shutdown the logger
	CLogger::Shutdown();
    return 0;
}
