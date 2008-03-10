#include "demo.h"
//Utility Headers
#include "shader_manager.h"
//Scene Headers
#include "scene.h"
#include "camera.h"
#include "background_layer.h"
#include "user_interface_layer.h"
#include "camera_layer.h"
#include "selection_layer.h"
#include "geometry_layer.h"
#include "compass.h"
#include "frame_rate_monitor.h"
//Geometry Headers
#include "nurbs_curve.h"
#include "curve_profile.h"


//Global scene object
WCScene *scene;
WCGeometryLayer *cLayer, *cpLayer;
WCCollection<WCControlPoint*> *surfCP;
WCCurveProfile* profile;
WCNurbsCurve* curve;


/********************************************************************************************************************/


int HandleModifiers(void) {
	int keys = glutGetModifiers();	
	if (keys & GLUT_ACTIVE_SHIFT) return 1;
	if (keys & GLUT_ACTIVE_CTRL) return 2;
	if (keys & GLUT_ACTIVE_ALT) return 3;
	return 0;
}


/********************************************************************************************************************/


void InitScene(void) {
	//Create the shader manager
	WCShaderManager::Initialize("../../shaders/shader_manifest.xml", "../../shaders/", false);
	//Create the scene
	scene = new WCScene();
	WCBackgroundLayer* backgroundLayer = new WCBackgroundLayer(scene);
	WCUserInterfaceLayer* uiLayer = new WCUserInterfaceLayer(scene);
	scene->RegisterLayer(backgroundLayer);
	scene->LayerBelow(backgroundLayer);
	scene->RegisterLayer(uiLayer);

	//Add in layers for various geometry objects
	cLayer = new WCGeometryLayer(scene, "Curves");
	cpLayer = new WCGeometryLayer(scene, "ControlPoints");
	scene->RegisterLayer(cLayer, backgroundLayer);
	scene->RegisterLayer(cpLayer, backgroundLayer);
	//Add a compass into the UI layer
	WCCompass *compass = new WCCompass(uiLayer);
	compass->SetPlacement(WCPlacement::LowerLeft());
	//Add a FRM into the UI layer
	WCFrameRateMonitor *frm = new WCFrameRateMonitor(uiLayer);
	frm->SetPlacement(WCPlacement::LowerRight());
}


void InitGeometry(void) {
	
	/*** Test 1 ***/
	
	//Create a curve profile
	profile = new WCCurveProfile();
	cLayer->RegisterGeometricObject(profile);
	
	//Setup control points
	WCControlPoint *p0 = new WCControlPoint(0.0, 0.0, 0.0);
	WCControlPoint *p1 = new WCControlPoint(0.0, 1.0, 0.0);
	WCControlPoint *p2 = new WCControlPoint(1.75, 1.0, 0.0);
	WCControlPoint *p3 = new WCControlPoint(2.0, 1.0, 0.0, 0.707106781187);
//	WCControlPoint *p3 = new WCControlPoint(2.0, 1.0, 0.0);
	WCControlPoint *p4 = new WCControlPoint(2.0, 0.75, 0.0);
	WCControlPoint *p5 = new WCControlPoint(2.0, 0.0, 0.0);
	//Register control points
	cpLayer->RegisterGeometricObject(p0);
	cpLayer->RegisterGeometricObject(p1);
	cpLayer->RegisterGeometricObject(p2);
	cpLayer->RegisterGeometricObject(p3);
	cpLayer->RegisterGeometricObject(p4);
	cpLayer->RegisterGeometricObject(p5);
	
	//Create a collection
	WCCollection<WCControlPoint*> cpCol;
	
	//Define curve 0
	cpCol.Add(p0);
	cpCol.Add(p1);
	curve = new WCNurbsCurve(1, &cpCol, WCNurbsMode::Default());
	profile->AddCurve(curve);
	cpCol.Clear();
	
	//Define curve 1
	cpCol.Add(p1);
	cpCol.Add(p2);
	curve = new WCNurbsCurve(1, &cpCol, WCNurbsMode::Default());
	profile->AddCurve(curve);
	cpCol.Clear();

	//Define curve 2
	cpCol.Add(p2);
	cpCol.Add(p3);
	cpCol.Add(p4);
	curve = new WCNurbsCurve(2, &cpCol, WCNurbsMode::Default());
	profile->AddCurve(curve);
	cpCol.Clear();

	//Define curve 3
	cpCol.Add(p4);
	cpCol.Add(p5);
	curve = new WCNurbsCurve(1, &cpCol, WCNurbsMode::Default());
	profile->AddCurve(curve);
	cpCol.Clear();

	//Define curve 3
	cpCol.Add(p5);
	cpCol.Add(p0);
	curve = new WCNurbsCurve(1, &cpCol, WCNurbsMode::Default());
	profile->AddCurve(curve);
	cpCol.Clear();

	/*** Test 3 ***/

	std::cout << "0.00: " << profile->Evaluate(0.0) << std::endl;
	std::cout << "0.19: " << profile->Evaluate(0.19) << std::endl;
	std::cout << "0.20: " << profile->Evaluate(0.20) << std::endl;
	std::cout << "0.21: " << profile->Evaluate(0.21) << std::endl;
	std::cout << "0.50: " << profile->Evaluate(0.50) << std::endl;
	std::cout << "0.60: " << profile->Evaluate(0.60) << std::endl;
	std::cout << "0.80: " << profile->Evaluate(0.80) << std::endl;
	std::cout << "0.90: " << profile->Evaluate(0.90) << std::endl;
	std::cout << "0.91: " << profile->Evaluate(0.91) << std::endl;
	std::cout << "0.92: " << profile->Evaluate(0.92) << std::endl;
	std::cout << "0.93: " << profile->Evaluate(0.93) << std::endl;
	std::cout << "0.94: " << profile->Evaluate(0.94) << std::endl;
	std::cout << "0.99: " << profile->Evaluate(0.99) << std::endl;
	std::cout << "1.00: " << profile->Evaluate(1.00) << std::endl;
	
	/*** Test 4 ***/
	
	/*** Test 5 ***
	
	/*** Test 6 ***
	
	
	/*** End Test ***/
	
	std::cout << *profile << std::endl;
}


void Display(void) {
	scene->Render();
	//Swap out the back buffer to the front	(TODO: Abstract for platform independence)
	glutSwapBuffers();
}


void KeyPress(unsigned char k, int x, int y) {
	switch(k) {
		//View switches
		case '1': scene->ActiveCamera()->AlignXAxis(); break;
		case '2': scene->ActiveCamera()->AlignYAxis(); break;
		case '3': scene->ActiveCamera()->AlignZAxis(); break;
		case '4': scene->ActiveCamera()->AlignThreeQuarters(); break;
		case '5': scene->ActiveCamera()->FitToWindow(); break;
		case 'g' : cpLayer->IsVisible( !cpLayer->IsVisible() ); break;
		case 'p' : cLayer->IsVisible( !cLayer->IsVisible() ); break;
		case 'b': std::cout << profile->BoundingBox() << std::endl; break;
	}
	//Render the scene if it's dirty
	if (scene->IsDirty()) Display();
}


void SpecialKeyPress(int key, int x, int y) {
	//Handle modifier key presses
	HandleModifiers();	
	std::cout << "Special Key Press: " << key << std::endl;
	//Render the scene if it's dirty
	if (scene->IsDirty()) Display();
}


void Resize(int w, int h) {
	//Set the window width and height parameters
	scene->OnReshape(w, h);
	//Render the scene if it's dirty
	if (scene->IsDirty()) Display();
}


void MousePress(int button, int state, int x, int y) {
	//State 0 is down
	if (state == 0) {
		//Handle modifier key presses
		int m = HandleModifiers();	
		//Switch camera as needed
		switch(m) {
			case 0: //Selection Case
				break;
			case 1: //Rotate Case
				scene->LayerAbove( scene->CameraLayer());
				scene->CameraLayer()->Mode(WCCameraMode::Rotate());
				break;
			case 2: //Pan Case
				scene->LayerAbove( scene->CameraLayer());
				scene->CameraLayer()->Mode(WCCameraMode::Pan());
				break;
			case 3: //Zoom Case
				scene->LayerAbove( scene->CameraLayer());
				scene->CameraLayer()->Mode(WCCameraMode::Zoom());
				break;
		}	
		scene->OnMouseDown(WCMouseButton::Left());
	}
	//State 1 is up
	else {
		scene->LayerBelow( scene->CameraLayer());
		scene->OnMouseUp(WCMouseButton::Left());
	}
	//Render the scene if it's dirty
	if (scene->IsDirty()) Display();
}


void MouseMove(int x, int y) {
	//React to a mouse movement
	scene->OnMouseMove(x, y);
	//Render the scene if it's dirty
	if (scene->IsDirty()) Display();
}


void PassiveMouseMove(int x, int y) {
	//React to a mouse movement
	scene->OnMouseMove(x, y);
	//Render the scene if it's dirty
	if (scene->IsDirty()) Display();	
}

void Idle(void) {
	//Call scene Idle method
	scene->OnIdle();
//	surface->IsDirty(true);	
	//Render the scene if it's dirty
	if (scene->IsDirty()) Display();
}


int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitWindowSize(512, 512);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	glutCreateWindow("Wildcat Scene Rendering");

	InitScene();
	InitGeometry();

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
	//Time to exit
    return 0;
}