/*******************************************************************************
* Copyright (c) 2007, 2008, CerroKai Development
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of CerroKai Development nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY CerroKai Development ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL CerroKai Development BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************************/


/*** Included Header Files ***/
#include "Sketcher/sketch.h"
#include "Sketcher/sketch_feature.h"
#include "Sketcher/sketch_profile.h"
#include "Kernel/document.h"


/***********************************************~***************************************************/


bool WCSketch::IsConnectedToProfile(std::list<WCGeometricCurve*> &list, WCGeometricCurve *curve) {
	std::list<WCGeometricCurve*>::iterator curveIter;
	WCVector4 begin = curve->Evaluate(0.0);
	WCVector4 end = curve->Evaluate(1.0);
	WCVector4 pt;
	WPFloat dist;
	
	//Loop through all curves, try to match end points
	for (curveIter = list.begin(); curveIter != list.end(); curveIter++) {
		pt = (*curveIter)->Evaluate(0.0);
		//Beginning - Beginning match
		dist = begin.Distance(pt);
		if (dist < SKETCH_ANALYSIS_TOLERANCE) return true;
		//Ending - Beginning match
		dist = end.Distance(pt);
		if (dist < SKETCH_ANALYSIS_TOLERANCE) return true;
		pt = (*curveIter)->Evaluate(1.0);
		//Beginning - Ending match
		dist = begin.Distance(pt);
		if (dist < SKETCH_ANALYSIS_TOLERANCE) return true;
		//Ending - Ending match
		dist = end.Distance(pt);
		if (dist < SKETCH_ANALYSIS_TOLERANCE) return true;
	}
	//Connection not found
	return false;
}


/***********************************************~***************************************************/


void WCSketch::Analyze(void) {
	//Perform constrain, projection, and geometry analysis
//	CLOGGER_WARN(WCLogManager::RootLogger(), "****************** Sketch Analysis ******************");
	this->AnalyzeProjections();
	this->AnalyzeGeometry();
//	CLOGGER_WARN(WCLogManager::RootLogger(), "****************** Sketch Analysis ******************");
}


void WCSketch::AnalyzeConstraints(void) {
	CLOGGER_INFO(WCLogManager::RootLogger(), "WCSketch::AnalyzeConstraints - Analyzing Sketch Constraints");
	std::cout << *(this->_planner);
	//Solve constraint system
	WCConstraintPlan *plan = NULL;
	if (this->_planner->IsDirty()) {
		plan = this->_planner->Solve();
		if (plan != NULL) std::cout << *plan;
	}
	//Alter geometry by solution
	//...
	//Clean up
	if (plan != NULL) delete plan;
}


void WCSketch::AnalyzeProjections(void) {
	//Not sure what to do here
}


void WCSketch::AnalyzeGeometry(void) {
	//Analyze points, lines, curves, and profiles
	this->AnalyzeProfiles();
	this->AnalyzePoints();
	this->AnalyzeLines();
	this->AnalyzeCurves();
}


void WCSketch::AnalyzePoints(void) {
	std::map<WCGeometricPoint*,WCEventController*>::iterator pointIter;
	WCSketchFeature *feature;
	//Go through all points
	for (pointIter = this->_pointMap.begin(); pointIter != this->_pointMap.end(); pointIter++) {
		if (!(*pointIter).first->IsConstruction()) {
			feature = dynamic_cast<WCSketchFeature*>( (*pointIter).second->Associate() );
			CLOGGER_WARN(WCLogManager::RootLogger(), "Point " << feature->GetName() << " is not a construction point.");
		}
	}
}


void WCSketch::AnalyzeLines(void) {
	//Look for lines that are in profiles alone
	//...
}


void WCSketch::AnalyzeCurves(void) {
	//Look for curves that intersect themselves (not at start-end point)
}


void WCSketch::AnalyzeProfiles(void) {
	//Place all lines and curves into WCGeometricCurve* list
	std::list<WCGeometricCurve*> curveList;
	//Go through all lines first
	std::map<WCGeometricLine*,WCEventController*>::iterator lineMapIter;
	for (lineMapIter = this->_lineMap.begin(); lineMapIter != this->_lineMap.end(); lineMapIter++) {
		//Make sure line is not construction
		if (!(*lineMapIter).first->IsConstruction()) {
//			std::cout << (*lineMapIter).first << std::endl;
			curveList.push_back((*lineMapIter).first);
		}
	}
	//Go through curves next
	std::map<WCNurbsCurve*,WCEventController*>::iterator curveMapIter;	
	for (curveMapIter = this->_curveMap.begin(); curveMapIter != this->_curveMap.end(); curveMapIter++) {
		//Make sure curve is not construction
		if (!(*curveMapIter).first->IsConstruction()) {
//			std::cout << (*curveMapIter).first << std::endl;
			curveList.push_back((*curveMapIter).first);
		}
	}

	//List of all curve profiles
	std::list< std::list<WCGeometricCurve*> > curveProfilesList;
	std::list< std::list<WCGeometricCurve*> >::iterator prevList;
	std::list< std::list<WCGeometricCurve*> >::iterator curveProfilesIter;

	//Go through every curve in the list
	std::list<WCGeometricCurve*>::iterator curveIter;
	for (curveIter = curveList.begin(); curveIter != curveList.end(); curveIter++) {
		//Go through every current profile
		prevList = curveProfilesList.end();
		for (curveProfilesIter = curveProfilesList.begin(); curveProfilesIter != curveProfilesList.end(); curveProfilesIter++) {
			//Is the curve coonected to this list of curves
			if (this->IsConnectedToProfile(*curveProfilesIter, *curveIter)) {
				if (prevList != curveProfilesList.end()) {
					//Merge the lists
					(*prevList).merge(*curveProfilesIter);
					//Remove current list from master list
					curveProfilesList.erase(curveProfilesIter);
				}
				else {
					//Add the curve into the new list
					(*curveProfilesIter).push_back(*curveIter);
					//Set prevList
					prevList = curveProfilesIter;
				}
			}
		}
		//If prevList is end - no list found
		if (prevList == curveProfilesList.end()) {
			//Create new list and add it in to master list
			curveProfilesList.push_back( std::list<WCGeometricCurve*>(1, *curveIter) );
		}
	}
	
	//Holding place for pending profiles
	WCSketchProfile *profile;
	std::list<WCSketchProfile*> newProfilesList;
	std::list<WCSketchProfile*>::iterator profileListIter;
	bool matched;
	
//	std::cout << "Printing all exsiting profiles\n";
//	for (profileListIter = this->_profileList.begin(); profileListIter != this->_profileList.end(); profileListIter++)
//		std::cout << *(*profileListIter);
	
	//Process list of new profiles
	for (curveProfilesIter = curveProfilesList.begin(); curveProfilesIter != curveProfilesList.end(); curveProfilesIter++) {
		//See if there is an exact match to existing profiles
		matched = false;
		profileListIter = this->_profileList.begin();
		while (profileListIter != this->_profileList.end()) {
//		for (profileListIter = this->_profileList.begin(); profileListIter != this->_profileList.end(); profileListIter++) {
			//If there is a match....
			if ((*(*profileListIter)) == (*curveProfilesIter)) {
//				std::cout << "Found a profile match: " << std::endl;
				matched = true;
				//Add matched existing profile for keeping
				newProfilesList.push_back( *profileListIter );
				//Remove existing profile from main list
				this->_profileList.erase(profileListIter);
				//Jump to the end
				profileListIter = this->_profileList.end();
			}
			//Otherwise, move on to next profileList item
			else {
				profileListIter++;
			}
		}

		//If not matched create new profile
		if (matched == false) {
			profile = new WCSketchProfile(this, "", *curveProfilesIter);
//			std::cout << "Adding profile: " << profile << std::endl;;
			//Add into the sketch list
			newProfilesList.push_back( profile );
		}
	}
	
	//Remove all non-matched old profiles
//	std::cout << "Remaining profiles: " << this->_profileList.size() << std::endl;
	for (profileListIter = this->_profileList.begin(); profileListIter != this->_profileList.end(); profileListIter++) {
//		std::cout << "Removing existing profile: " << *profileListIter << std::endl;
		//Delete unneeded profile
		delete (*profileListIter);
	}
	//Clear the list
	this->_profileList.clear();
	
	//Insert all new and matched profiles into the sketch
	for (profileListIter = newProfilesList.begin(); profileListIter != newProfilesList.end(); profileListIter++) {
		this->_profileList.push_back( *profileListIter );
	}
}


/***********************************************~***************************************************/


void ShowLists(std::list< std::list<WCGeometricCurve*> > &profiles) {
	std::list< std::list<WCGeometricCurve*> >::iterator profilesIter;
	std::list<WCGeometricCurve*>::iterator iter;
	for (profilesIter = profiles.begin(); profilesIter != profiles.end(); profilesIter++) {
		std::cout << &(*profilesIter) << std::endl;
		for (iter = (*profilesIter).begin(); iter != (*profilesIter).end(); iter++) {
			std::cout << "\t" << (*iter) << std::endl;
		}
	}
}


/***********************************************~***************************************************/

