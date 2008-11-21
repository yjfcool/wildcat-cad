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
#include "point.h"
#include "control_point.h"
#include "nurbs_surface.h"


//Global scene object
WCScene *scene;
WCGeometryLayer *pLayer, *cLayer, *sLayer, *cpLayer;
WCNurbsSurface *surface;
WCCollection<WCControlPoint*> *surfCP;


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
	//Create the shader manager
	WCShaderManager::Initialize("../../shaders/shader_manifest.xml", "../../shaders/", true);
	//Create the scene
	scene = new WCScene();
	WCBackgroundLayer* backgroundLayer = new WCBackgroundLayer(scene);
	WCUserInterfaceLayer* uiLayer = new WCUserInterfaceLayer(scene);
	scene->RegisterLayer(backgroundLayer);
	scene->LayerBelow(backgroundLayer);
	scene->RegisterLayer(uiLayer);

	//Add in layers for various geometry objects
	pLayer = new WCGeometryLayer(scene, "Points");
	cLayer = new WCGeometryLayer(scene, "Curves");
	sLayer = new WCGeometryLayer(scene, "Surfaces");
	cpLayer = new WCGeometryLayer(scene, "ControlPoints");
	scene->RegisterLayer(pLayer, backgroundLayer);
	scene->RegisterLayer(cLayer, backgroundLayer);
	scene->RegisterLayer(sLayer, backgroundLayer);
	scene->RegisterLayer(cpLayer, backgroundLayer);
	//Add a compass into the UI layer
	WCCompass *compass = new WCCompass(uiLayer);
//	compass->SetPlacement(-3, -3);
	compass->SetPlacement(WCPlacement::LowerLeft());
	//Add a FRM into the UI layer
	WCFrameRateMonitor *frm = new WCFrameRateMonitor(uiLayer);
//	frm->SetPlacement(2, -2.5);
	frm->SetPlacement(WCPlacement::LowerRight());
}


void Display(void) {
	scene->Render();
	//Swap out the back buffer to the front	(TODO: Abstract for platform independence)
	glutSwapBuffers();
}


void KeyPress(unsigned char k, int x, int y) {
	switch(k) {
		//Point movement
		case 'w': surfCP->At(6)->Z( surfCP->At(6)->Z() + 0.1 ) ; break;
		case 's': surfCP->At(6)->Z( surfCP->At(6)->Z() - 0.1 ); break;
		//View switches
		case '1': scene->ActiveCamera()->AlignXAxis(); break;
		case '2': scene->ActiveCamera()->AlignYAxis(); break;
		case '3': scene->ActiveCamera()->AlignZAxis(); break;
		case '4': scene->ActiveCamera()->AlignThreeQuarters(); break;
		case '5': scene->ActiveCamera()->FitToWindow(); break;
		//Color switches
		case '6': surface->Color( WCColor(1.0, 0.8, 0.8, 1.0) ); break;
		case '7': surface->Color( WCColor(0.45, 0.75, 0.45, 1.0) ); break;
		//Shader switches
		case '8': sLayer->RenderProgram(WCShaderManager::ProgramFromName("scn_basiclight")); break;
		case '9': sLayer->RenderProgram(WCShaderManager::ProgramFromName("scn_gooch")); break;
		case '0': sLayer->RenderProgram(WCShaderManager::ProgramFromName("scn_hemisphere")); break;
		// Other switches
		case ']': surface->Degree(surface->DegreeU() + 1, surface->DegreeV() + 1); break;
		case '[': surface->Degree(surface->DegreeU() - 1, surface->DegreeV() - 1); break;
		case '=': surface->LevelOfDetail(surface->LevelOfDetailU() + 1, surface->LevelOfDetailV() + 1); break;
		case '-': surface->LevelOfDetail(surface->LevelOfDetailU() - 1, surface->LevelOfDetailV() - 1); break;	
		case 'l':
			if (scene->ActiveCamera()->RenderState()->PolygonMode() == GL_LINE)
				scene->ActiveCamera()->RenderState()->PolygonMode(GL_FILL);
			else
				scene->ActiveCamera()->RenderState()->PolygonMode(GL_LINE);
			break;	
		case 'g' :
			WCLayer* layer = scene->LayerFromName("ControlPoints");
			if (layer != NULL) 
				layer->IsVisible( !layer->IsVisible() );
			break;
		case 'p':
			std::cout << surface << std::endl;
			break;
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

	InitPart();

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