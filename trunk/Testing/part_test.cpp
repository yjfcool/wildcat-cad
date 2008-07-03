//Headers
#include "demo.h"
//Utility Headers
#include "shader_manager.h"
#include "log_manager.h"
//Part Headers
#include "part_workbench.h"
#include "part.h"
//Scene Headers
#include "scene.h"
#include "camera.h"
#include "layer.h"
#include "camera_layer.h"
#include "compass.h"
#include "frame_rate_monitor.h"


//Global scene object
//WCPart *part;
//WCScene *scene;
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


void InitEnvironment(void) {
	
}

void InitPart(void) {
	//Initialize logger manager
	WCLogManager::Initialize();
	//Create the shader manager
	WCShaderManager::Initialize("../../shaders/shader_manifest.xml", "../../shaders/", false);
	
	//Create the part
	doc = new WCPart("", "");
	workbench = new WCPartWorkbench(part);
	scene = part->Scene();
	//Create UI objects
	WCCompass *compass = new WCCompass(part->UserInterfaceLayer());
	compass->SetPlacement(WCPlacement::LowerLeft());
	WCFrameRateMonitor *frm = new WCFrameRateMonitor(part->UserInterfaceLayer());
	frm->SetPlacement(WCPlacement::LowerRight());
//	frm->IsVisible(true);
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
		// Other switches
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
	glutInitWindowSize(1024, 768);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	glutCreateWindow("Wildcat Scene Rendering");

	InitEnvironment();
	InitPart();
	//Show some info about the document
	std::cout << *part << std::endl;

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