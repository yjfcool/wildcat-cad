/*** Included Header Files ***/
#include "wildcat_app.h"
#include "scene.h"
#include "log_manager.h"


WCWildcatApp *g_App;


/***********************************************~***************************************************/


//Called on window redraw
void GLDisplay(void) {
	g_App->Scene()->Render();
}            
    

//Called on window resize          
void GLReshape(int w, int h) {        
	g_App->Scene()->OnReshape(w, h);
}                       


void GLSpecialKey(int key, int x, int y) {
//	int mod = glutGetModifiers();
	//Pass special key press on to scene
	g_App->Scene();
}				


void  GLMouseMove(int x, int y) {
	g_App->Scene()->OnMouseMove(x, y);
}


void GLMousePress(int button, int state, int x, int y) {
	//If the button is being pressed down
}															    
 
			             
void GLKeyPress(unsigned char key, int x, int y) {            
       
}            


/***********************************************~***************************************************/


WCWildcatApp::WCWildcatApp(int argc, char **argv) : ::WCObject() {
	//Start up logger
	WCLogManager::Initialize(WILDCATAPP_DEFAULT_LOGGER);
	
	//Set up the scene and scene objects
	this->_scene = new WCScene();	
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_ALPHA);
	glutInitWindowSize(400, 400);
	glutInitWindowPosition(550, 100);
	glutCreateWindow(argv[0]);
	
	g_App = this;
	      
	glutDisplayFunc(GLDisplay);
	glutReshapeFunc(GLReshape);
	glutKeyboardFunc(GLKeyPress);
	glutSpecialFunc(GLSpecialKey);
	glutPassiveMotionFunc(GLMouseMove);
	glutMotionFunc(GLMouseMove);
    glutMouseFunc(GLMousePress);
}


WCWildcatApp::~WCWildcatApp() {
	//Check to make sure scene is not null
	if (this->_scene != NULL) delete this->_scene;
	
	//Shut down logger
	WCLogManager::Terminate();
}



void WCWildcatApp::Run(void) {
	//Just pass control to glut
	glutMainLoop();
}


/***********************************************~***************************************************/

	