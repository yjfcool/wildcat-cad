//Headers
#include "demo.h"
//Utility Headers
#include "shader_manager.h"
#include "log_manager.h"
//Scene Headers
#include "camera.h"
#include "selection_mode.h"
//Part Headers
#include "workbench.h"
#include "part.h"
#include "part_body.h"
#include "plane.h"
#include "part_pad.h"
#include "part_shaft.h"
//Sketch Headers
#include "sketch.h"
#include "grid.h"
#include "sketch_arc.h"
#include "sketch_axis.h"
#include "sketch_circle.h"
#include "sketch_conic_twopoint.h"
#include "sketch_ellipse.h"
#include "sketch_line.h"
#include "sketch_point.h"
#include "sketch_rectangle.h"
#include "sketch_constraint.h"
#include "sketch_trim.h"
#include "constraint_fix.h"


//Global object
//WCPartWorkbench *workbench;
WCDocument *doc;


/********************************************************************************************************************/


int HandleModifiers(void) {
	int keys = glutGetModifiers();	
	if (keys & GLUT_ACTIVE_SHIFT) return 1;
	if (keys & GLUT_ACTIVE_CTRL) return 2;
	if (keys & GLUT_ACTIVE_ALT) return 3;
	return 0;
}


/********************************************************************************************************************/


void InitPart(void) {
	//Initialize logger manager
	WCLogManager::Initialize();
	//Initialize xml manager
	XMLPlatformUtils::Initialize();
	//Create the shader manager
	WCShaderManager::Initialize("../../shaders/shader_manifest.xml", "../../shaders/", false);
	
	//Create the part
	doc = new WCPart("", "");

	WCPartFeature::DefaultSurfaceRenderer = WCShaderManager::ProgramFromName("scn_basiclight");
}


void ShutdownPart(void) {
	//Terminate the managers
	WCShaderManager::Terminate();
	XMLPlatformUtils::Terminate();
	WCLogManager::Terminate();
}

void Display(void) {
	doc->ActiveWorkbench()->Render();
	//Swap out the back buffer to the front	(TODO: Abstract for platform independence)
	glutSwapBuffers();
}


void KeyPress(unsigned char k, int x, int y) {
//	printf("Key: %d\n", k);
	//Take care of arrow keys
//	if ((k >= 100) && (k <= 103)) {
//		switch(k) {
//			case 100: doc->ActiveWorkbench()->OnArrowKeyPress(WCArrowKey::Left()); break;
//			case 101: doc->ActiveWorkbench()->OnArrowKeyPress(WCArrowKey::Up()); break;
//			case 102: doc->ActiveWorkbench()->OnArrowKeyPress(WCArrowKey::Right()); break;
//			case 103: doc->ActiveWorkbench()->OnArrowKeyPress(WCArrowKey::Down()); break;
//		}
//		//Render the doc if it's dirty
//		if (doc->IsDirty()) Display();
//		return;
//	}
	
	//Declare some stuff
	static bool bPress = false;
	WCPart* part = dynamic_cast<WCPart*>(doc);
	
	//Get controller for top selected object
	WCSelectionObject* selected = doc->ActiveWorkbench()->SelectionManager()->Top();
	WCEventController* controller = dynamic_cast<WCEventController*>(selected);

	WCDrawingMode *mode;
	WCAction *action;
	
	//Switch based on active workbench
	if (doc->ActiveWorkbench()->Name() == "Sketcher") {
		//Get the sketch workbench and sketch
		WCSketchWorkbench *sketchWB = dynamic_cast<WCSketchWorkbench*>(doc->ActiveWorkbench());
		WCSketch *sketch = sketchWB->Sketch();
		std::list<WCSketchFeature*> features = doc->ActiveWorkbench()->SelectionManager()->FilterSelected<WCSketchFeature>(true);
		
		switch(k) {
			case 'a':
				//Create a new drawing mode
				mode = WCSketchArc::ModeCreate( sketchWB, WCSketchArcType::Standard() );
				//Go into arc mode
				doc->ActiveWorkbench()->DrawingMode(mode);
				break;
			case 'c':
				//Create a new drawing mode
				mode = WCSketchCircle::DrawingMode( sketchWB );
				//Go into circle mode
				doc->ActiveWorkbench()->DrawingMode(mode);
				break;
			case 'd':
				mode = WCSketchConstraint::ModeCreateDimensional( sketchWB );
				//Go into dimensional constraint mode
				doc->ActiveWorkbench()->DrawingMode(mode);
				break;
			case 'e':
				//Create a new drawing mode
				mode = WCSketchEllipse::ModeCreate( sketchWB );
				//Go into ellipse mode
				doc->ActiveWorkbench()->DrawingMode(mode);
				break;
			case 'f':
				//Fix the sketch feature
				if (features.size() == 1) {
					action = WCConstraintFix::ActionCreate(sketch, "", features.front());
					doc->ExecuteAction( action );
				}
				break;
			case 'g':
				mode = WCSketchConstraint::ModeCreateGeometric( sketchWB );
				//Go into geometric constraint mode
				doc->ActiveWorkbench()->DrawingMode(mode);
				break;
			case 'h':
				//Hide constraints
				sketch->DrawConstraints( !sketch->DrawConstraints() );
				break;
			case 'i':
				mode = WCSketchTrim::ModeCreate( sketchWB );
				//Go into trim mode
				doc->ActiveWorkbench()->DrawingMode(mode);
				break;
			case 'l':
				mode = WCSketchLine::ModeCreate( sketchWB );
				//Go into line mode
				doc->ActiveWorkbench()->DrawingMode(mode);
				break;
			case 'o':
				mode = WCSketchConicTwoPoint::ModeCreate( sketchWB );
				//Go into two-point conic mode
				doc->ActiveWorkbench()->DrawingMode(mode);
				break;
			case 'p':
				//Create a new drawing mode
				mode = WCSketchPoint::ModeCreate( sketchWB );
				//Go into point mode
				doc->ActiveWorkbench()->DrawingMode(mode);
				break;
			case 'q':
				//Exit sketcher
				doc->ExitWorkbench();
				break;
			case 'r':
				//Create a new drawing mode
				mode = WCSketchRectangle::ModeCreate( sketchWB );
				//Go into rectangle mode
				doc->ActiveWorkbench()->DrawingMode(mode);
				break;
			case 's':
				//Swap suggestion snap
				if (sketchWB->IsSuggestionSnap()) {
					std::cout << "Turning All Snap Off\n";
					sketchWB->IsSuggestionSnap(false);
					sketchWB->Grid()->IsSnap(false);
				}
				else {	
					std::cout << "Turning All Snap On\n";
					sketchWB->IsSuggestionSnap(true);
					sketchWB->Grid()->IsSnap(true);				
				}
				break;
			case 't':
				//Create a new drawing mode
				mode = WCSketchArc::ModeCreate( sketchWB, WCSketchArcType::ThreePoint() );
				//Go into three-point arc mode
				doc->ActiveWorkbench()->DrawingMode(mode);
				break;
			case 'w':
				//Switch the construction mode
				sketchWB->IsConstruction( !sketchWB->IsConstruction() );
				break;
			case 'x':
				//Create a new drawing mode
				mode = WCSketchAxis::ModeCreate( sketchWB );
				//Go into axis mode
				doc->ActiveWorkbench()->DrawingMode(mode);
			case 'z':
				//Analyze the sketch
				sketch->Analyze();
				break;
			case '1':
				//Mark all selected as not construction
				action = WCSketch::ActionMarkNonConstruction(sketch, doc->ActiveWorkbench()->SelectionManager()->FilterSelected<WCSketchFeature>(true));
				doc->ExecuteAction( action );
				break;
			case '2':
				//Mark all selected as construction
				action = WCSketch::ActionMarkConstruction(sketch, doc->ActiveWorkbench()->SelectionManager()->FilterSelected<WCSketchFeature>(true));
				doc->ExecuteAction( action );
				break;
			case '3':
				//Turn on-off auto-constraint generation
				sketchWB->IsGeometricConstraint( !sketchWB->IsGeometricConstraint() );
				sketchWB->IsDimensionalConstraint( !sketchWB->IsDimensionalConstraint() );
				break;
			case '8': doc->ActiveWorkbench()->ZoomToFit(); 
				break;
			case 27:
				//Revert to default drawing mode
				doc->ActiveWorkbench()->DrawingMode( WCDrawingMode::Selection( doc->ActiveWorkbench() ));
				break;
			case 127:
				//Delete selected sketch features
				action = WCSketch::ActionDelete(sketch, doc->ActiveWorkbench()->SelectionManager()->FilterSelected<WCSketchFeature>(true) );
				doc->ExecuteAction( action );
				break;
			case '-': std::cout << *doc->ActiveWorkbench()->SelectionManager(); break;
			case '=': WCObject::DebugReport(); break;
		}
	}
	else if (doc->ActiveWorkbench()->Name() == "Part Designer") {
		//Do some casting, just in case
		WCSketch *sketch;
		if (controller != NULL) sketch = dynamic_cast<WCSketch*>(controller->Associate());
		else sketch == NULL;
		WCPartWorkbench *partWB = dynamic_cast<WCPartWorkbench*>(doc->ActiveWorkbench());
		switch(k) {
			case 'b':
				if (bPress) break;
				doc->ExecuteAction( WCBody::ActionCreate(part, "") );
				bPress = true;
				break;
			case 'e':
				if ((controller == NULL) || (doc->ActiveWorkbench()->SelectionManager()->Count() != 1)) {
					std::cout << "Must have one sketch selected to enter sketcher workbench.\n";
					return;
				}
				//See if selected item is a sketch
				if (sketch == NULL) { std::cout << "A sketch is not selected.\n"; break; }
				doc->EnterWorkbench(sketch->Workbench());
				break;
			case 'h':
				//Create a new drawing mode
				mode = WCShaft::ModeCreate(partWB);
				//Go into shaft mode
				doc->ActiveWorkbench()->DrawingMode(mode);		
				break;				
			case 'p': {
				//Create a new drawing mode
				mode = WCPad::ModeCreate(partWB);
				//Go into pad mode
				doc->ActiveWorkbench()->DrawingMode(mode);		
				break; }
			case 'q': 
				//Delete the part document
//				delete doc;
				//See about memory leaks
//				WCObject::DebugReport();
				//Quit the application 
//				exit(0);
				break;
			case 's':
				if (controller == NULL) break;
				WCPlane *refPlane = dynamic_cast<WCPlane*>(controller->Associate());
				if (refPlane != NULL) doc->ExecuteAction( WCSketch::ActionCreate(part->Body(), "", refPlane) ) ; 
				else { std::cout << "Must have one plane selected to create a sketch.\n"; }
				break;
			case 'v':
				//Save as
				doc->ActiveWorkbench()->SaveAs("test.xml");
				break;
			case '1': doc->ActiveWorkbench()->NamedView("Front"); break;
			case '2': doc->ActiveWorkbench()->NamedView("Back"); break;
			case '3': doc->ActiveWorkbench()->NamedView("Left"); break;
			case '4': doc->ActiveWorkbench()->NamedView("Right"); break;
			case '5': doc->ActiveWorkbench()->NamedView("Top"); break;
			case '6': doc->ActiveWorkbench()->NamedView("Bottom"); break;
			case '7': doc->ActiveWorkbench()->NamedView("Isometric"); break;
			case '8': doc->ActiveWorkbench()->ZoomToFit(); break;
			case '0': std::cout << *dynamic_cast<WCPart*>(doc) << std::endl; break;
			case 27:
				//Revert to default drawing mode
				doc->ActiveWorkbench()->DrawingMode( WCDrawingMode::Selection( doc->ActiveWorkbench() ));
				break;
			case '-': std::cout << *doc->ActiveWorkbench()->SelectionManager(); break;
			case '=': WCObject::DebugReport(); break;
			case ']': std::cout << *part; break;
		}
	}
	//Render the doc if it's dirty
	if (doc->IsVisualDirty()) Display();
}


void SpecialKeyPress(int key, int x, int y) {
	//Handle modifier key presses
	HandleModifiers();	
	std::cout << "Special Key Press: " << key << std::endl;
	//Render the doc if it's dirty
	if (doc->IsVisualDirty()) Display();
}


void Resize(int w, int h) {
	//Set the window width and height parameters
	doc->ActiveWorkbench()->OnReshape(w, h);
	//Render the doc if it's dirty
	if (doc->IsVisualDirty()) Display();
}


void MousePress(int button, int state, int x, int y) {
	//If left mouse button
	if (button == 0) {
		//State 0 is down
		if (state == 0) {
			//Handle modifier key presses
			int m = HandleModifiers();	
			//Switch camera as needed
			switch(m) {
				case 1: doc->ActiveWorkbench()->OnRotatePress(); break;
				case 2: doc->ActiveWorkbench()->OnPanPress();	break;
				case 3: doc->ActiveWorkbench()->OnZoomPress(); break;
				case 4: doc->ActiveWorkbench()->OnMultiSelectPress(); break;
			}	
			doc->ActiveWorkbench()->OnMouseDown(WCMouseButton::Left());
		}
		//State 1 is up
		else {
			doc->ActiveWorkbench()->OnZoomRelease();
			doc->ActiveWorkbench()->OnPanRelease();
			doc->ActiveWorkbench()->OnRotateRelease();
			doc->ActiveWorkbench()->OnMultiSelectRelease();
			doc->ActiveWorkbench()->OnMouseUp(WCMouseButton::Left());
		}
	}
	//Middle button
	if (button == 1) {
		if (state == 0)
			doc->ActiveWorkbench()->OnMouseDown(WCMouseButton::Middle());
		else
			doc->ActiveWorkbench()->OnMouseDown(WCMouseButton::Middle());
	}
	//Right button
	if (button == 2) {
		if (state == 0)
			doc->ActiveWorkbench()->OnMouseDown(WCMouseButton::Right());
		else
			doc->ActiveWorkbench()->OnMouseDown(WCMouseButton::Right());	}
	//Render the doc if it's dirty
	if (doc->IsVisualDirty()) Display();
}


void MouseMove(int x, int y) {
	//React to a mouse movement
	doc->ActiveWorkbench()->OnMouseMove(x, y);
	//Render the doc if it's dirty
	if (doc->IsVisualDirty()) Display();
}


void PassiveMouseMove(int x, int y) {
	//React to a mouse movement
	doc->ActiveWorkbench()->OnMouseMove(x, y);
	//Render the doc if it's dirty
	if (doc->IsVisualDirty()) Display();	
}

void Idle(void) {
	//Call Idle method
	doc->ActiveWorkbench()->OnIdle();
	//Render the doc if it's dirty
	if (doc->IsVisualDirty()) Display();
}


int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitWindowSize(1024, 768);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	glutCreateWindow("Wildcat Scene Rendering");

	InitPart();
	//Show some info about the document
//	std::cout << *(dynamic_cast<WCPart*>(doc)) << std::endl;

	glutDisplayFunc(Display);
	glutMouseFunc(MousePress);
	glutMotionFunc(MouseMove);
	glutPassiveMotionFunc(PassiveMouseMove);
	glutIdleFunc(Idle);
	glutKeyboardFunc(KeyPress);
	glutSpecialFunc(SpecialKeyPress);
	glutReshapeFunc(Resize);

	//Execute GLUT main loop and exit
	glutMainLoop();
	ShutdownPart();
	
	//Time to exit
    return 0;
}