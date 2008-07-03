#include "demo.h"
#include "adapter.h"
#include "nurbs_surface.h"
#include "bounding_object.h"


#include <CoreServices/CoreServices.h>


WCNurbsSurface *surface;
WCVector4 mark, move, spot;
WCControlPoint *_ipt, *pt;
WPFloat _tangentVal = 0.0;

/********************************************************************************************************************/


void InitOpenGL(void) {
	WCAdapter::Initialize();
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2, 0.2, 0.2, 1.0);
	options[OPTION_ENABLE_LIGHTING] = false;
	options[OPTION_DISPLAY_WIREFRAME] = true;
	//Setup fill mode
//	WCAdapter::PolygonMode(GL_LINE);
	srand ( time(NULL) );
	_ipt = new WCControlPoint(1.0, 1.0, 0.0);
}


void InitNurbs(void) {

	/*** Case 1 ***/
	
	WPFloat data[] = {
		//First row
		0.0,		0.0,		0.0, 1.0, 			
		0.6666666,	0.0,		0.0, 1.0,
		1.3333333,	0.0,		0.0, 1.0,
		2.0,		0.0,		0.0, 1.0,		
		//Second row
		0.0,		0.3333333,	0.0, 1.0, 			
		0.6666666,	0.3333333,	0.0, 1.0,
		1.3333333,	0.3333333,	1.0, 1.0,
		2.0,		0.3333333,	0.0, 1.0,		
		//Third row
		0.0,		0.6666666,	0.0, 1.0, 			
		0.6666666,	0.6666666,	0.0, 1.0,
		1.3333333,	0.6666666,	0.0, 1.0,
		2.0,		0.6666666,	0.0, 1.0,		
		//Fourth row
		0.0,		1.0,		0.0, 1.0, 			
		0.6666666,	1.0,		0.0, 1.0,
		1.3333333,	1.0,		0.0, 1.0,
		2.0,		1.0,		0.0, 1.0
	};	
	WCCollection<WCControlPoint*> *surfCP = WCControlPoint::CollectionFromArray(16, data);
	surface = new WCNurbsSurface(2, 2, 4, 4, surfCP, WCNurbsMode::Default(), WCNurbsMode::Default());
	std::cout << surface << std::endl;
//	surface->LevelOfDetail(100, 100);
	delete surfCP;
	
	/*** Case 2 ***

	WPFloat data[] = {
		//First row
		0.0,		0.0,		0.0, 1.0, 			
		2.0,		0.0,		0.0, 1.0,
		//Second row		
		0.0,		1.0,		0.0, 1.0, 			
		2.0,		1.0,		0.0, 1.0
	};	
	WCCollection<WCControlPoint*> *surfCP = WCControlPoint::CollectionFromArray(4, data);
	surface = new WCNurbsSurface(1, 1, 2, 2, surfCP, WCNurbsMode::Default(), WCNurbsMode::Default());
	std::cout << surface << std::endl;
	surface->LevelOfDetail(4, 2);
	delete surfCP;
	
	/*** End Cases ***/
}


void Display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glPolygonMode(GL_FRONT_AND_BACK, WCAdapter::PolygonMode());
 	
    //Wireframe control
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	/*** Frame Rate Counter ***/

	static int frameCounter = 0;
	static AbsoluteTime  start;
	static WPFloat fps = 0.0;	
	char str[12];
	sprintf(str, "%4.2f fps", fps);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glRasterPos2f(-1.8, -1.8);
	for (int i = 0; i < 10; i++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
	//Timer stuff
	if (frameCounter == 0) start = UpTime();	
	if (frameCounter == 100) {
		Nanoseconds elapsed = AbsoluteDeltaToNanoseconds(UpTime(), start);
		fps = 100.0 / UnsignedWideToUInt64(elapsed) * 1000000000;		
		frameCounter = 0;
	}
	else frameCounter++;	
	
	/*** ***/
	
	//Now go to viewing perspeective
	manipulator.applyTransform();

	//Time to dance
	WPFloat pert;
	if(!options[OPTION_ANIMATE]) {
		WCCollection<WCControlPoint*> *pts = surface->ControlPoints();
		for (int i=0; i<pts->Size(); i++) {
			pert = (rand() % 100) / 500.0 - 0.1;
//			std::cout << pert << std::endl;
			pts->At(i)->Z( pts->At(i)->Z() + pert );
		}
	}

	//Render the surface
	surface->Render(0);
	if (options[OPTION_TANGENT]) {
//		WCRay tangent = surface->Tangent(_tangentVal);
//		tangent.Render();
	}
	if (options[OPTION_INVERSION]) {
		WCVector4 inv = surface->PointInversion(*_ipt);
		glColor4f(0.0, 1.0, 1.0, 1.0);
		glPointSize(4.0);
		_ipt->Render(0);
		glBegin(GL_POINTS);
			glVertex3f(inv.I(), inv.J(), inv.K());
		glEnd();
		pt = _ipt;
	}

	//Swap buffers and start over
	glutSwapBuffers();
	glutReportErrors();
}


void Idle(void) {
	//manipulator.idle();
	surface->IsDirty(true);
	glutPostRedisplay();
}


void KeyPress(unsigned char k, int x, int y) {
    k = tolower(k);
    if (optionKeyMap.find(k) != optionKeyMap.end())
        options[optionKeyMap[k]] = !options[optionKeyMap[k]];

    switch(k) {
		case 's':
//			if (WCAdapter::PolygonMode() == GL_FILL) WCAdapter::PolygonMode(GL_LINE);
//			else WCAdapter::PolygonMode(GL_FILL);
			break;		
		//Degree Stuff
		case ']': surface->Degree(surface->DegreeU() + 1, surface->DegreeV() + 1); break;
		case '[': surface->Degree(surface->DegreeU() - 1, surface->DegreeV() - 1); break;
		case '=': surface->LevelOfDetail(surface->LevelOfDetailU() + 1, surface->LevelOfDetailV() + 1); break;
		case '-': surface->LevelOfDetail(surface->LevelOfDetailU() - 1, surface->LevelOfDetailV() - 1); break;
		//Tangent Stuff
		case '.': if (options[OPTION_TANGENT]) _tangentVal += 0.01; break;
		case ',': if (options[OPTION_TANGENT]) _tangentVal -= 0.01; break;
		//Point manipulators
		case 'z': if (pt != NULL) pt->Z( pt->Z() - 0.1 ); break;
		case 'e': if (pt != NULL) pt->Z( pt->Z() + 0.1 ); break;
		case 'a': if (pt != NULL) pt->X( pt->X() - 0.1 ); break;
		case 'd': if (pt != NULL) pt->X( pt->X() + 0.1 ); break;
		case 'x': if (pt != NULL) pt->Y( pt->Y() - 0.1 ); break;
		case 'w': if (pt != NULL) pt->Y( pt->Y() + 0.1 ); break;
		case 'k': surface->InsertKnotU(2.5, 1);	break;
		case 'r': delete surface; InitNurbs(); break;
		case 'm': surface->IsMouseOver( true ); break;
//		case 'b': surface->Bounds()->IsVisible( !surface->Bounds()->IsVisible() ); break;
	}
	//Force GLUT to redraw
	glutPostRedisplay();
}


void Resize(int w, int h) {
	//Set the window width and height parameters
//	WCAdapter::WindowWidth(w);
//	WCAdapter::WindowHeight(h);
	//Adjust the viewport
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
//    gluPerspective(60.0, (GLfloat)w/(GLfloat)h, 0.01, 10.0);
	glOrtho(-2, 2, -2, 2, -10, 10);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    manipulator.reshape(w, h);
}


void MousePress(int button, int state, int x, int y) {
	//State 0 is down
	if (state == 0) {
		manipulator.mouse(button, state, x, y);
	}
	//State 1 is up
	else {
		WCRay hitTest = WCRay::FromScreenCoords(x, y, 512, 512, 0.01, 10.0);
		pt = (WCControlPoint*)surface->HitTest(hitTest, 0.03 );
		if (pt != NULL) pt->IsSelected(true);
		else
			manipulator.mouse(button, state, x, y);	
	}
	//Force GLUT to redraw
	glutPostRedisplay();
	
}


void MouseMove(int x, int y) {
	manipulator.motion(x, y);
}


void PassiveMouseMove(int x, int y) {
//	if (!options[OPTION_ANIMATE]) {		
//	}
}



int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitWindowSize(512, 512);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	glutCreateWindow("NURBS Geometry Shader Rendering");

	InitOpenGL();
	InitNurbs();	

	manipulator.setDollyActivate( GLUT_LEFT_BUTTON, GLUT_ACTIVE_CTRL);
	manipulator.setPanActivate( GLUT_LEFT_BUTTON, GLUT_ACTIVE_SHIFT);
	manipulator.setDollyPosition( -3.0f);

	glutDisplayFunc(Display);
	glutMouseFunc(MousePress);
	glutMotionFunc(MouseMove);
	glutPassiveMotionFunc(PassiveMouseMove);
	glutIdleFunc(Idle);
	glutKeyboardFunc(KeyPress);
	glutReshapeFunc(Resize);

	//Configure the options
	optionKeyMap[' '] = OPTION_ANIMATE;
	options[OPTION_ANIMATE] = true;
	optionKeyMap['t'] = OPTION_TANGENT;
	options[OPTION_TANGENT] = false;
	optionKeyMap['i'] = OPTION_INVERSION;
	options[OPTION_INVERSION] = false;
	optionKeyMap['r'] = OPTION_REGENERATE;
	options[OPTION_REGENERATE] = true;

	//Execute GLUT main loop and exit
	glutMainLoop();
	//Time to exit
    return 0;
}