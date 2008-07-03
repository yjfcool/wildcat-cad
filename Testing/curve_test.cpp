#include "demo.h"
#include "adapter.h"
#include "nurbs_curve.h"
#include "curve_profile.h"
#include "trimmed_nurbs_surface.h"


#include <CoreServices/CoreServices.h>


WCNurbsCurve *curve, *c1, *c2, *c3, *cX;
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
	srand ( time(NULL) );
	_ipt = new WCControlPoint(1.0, 1.0, 0.0);
}


void InitNurbs(void) {
	/*** Test 1 ***

	WCControlPoint *p0 = new WCControlPoint(0.25, 0.25, 0.0);
	WCControlPoint *p1 = new WCControlPoint(0.25, 0.75, 0.0);
	WCControlPoint *p2 = new WCControlPoint(1.50, 0.75, 0.0);
	WCControlPoint *p3 = new WCControlPoint(1.75, 0.75, 0.0, 0.707106781187);
//	WCControlPoint *p3 = new WCControlPoint(1.75, 0.75, 0.0);
	WCControlPoint *p4 = new WCControlPoint(1.75, 0.50, 0.0);
	WCControlPoint *p5 = new WCControlPoint(1.75, 0.25, 0.0);
	WCControlPoint *p6 = new WCControlPoint(1.75, 0.0, 0.0);
	WCControlPoint *p7 = new WCControlPoint(1.75, -0.25, 0.0);
	WCCollection<WCControlPoint*> cpCol;	
	cpCol.Add(p0);	
	cpCol.Add(p1);
	cpCol.Add(p2);
	cpCol.Add(p3);
	cpCol.Add(p4);
	cpCol.Add(p5);
	cpCol.Add(p6);
	cpCol.Add(p7);		

	WPFloat data[12] = {0.0, 0.0, 0.0, 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 5.0, 5.0, 5.0};
	WCCollection<WPFloat>* kpCol = WCCollection<WPFloat>::CollectionFromArray(12, data);
//	curve = new WCNurbsCurve(3, &cpCol, WCNurbsMode::Custom(), kpCol);
	curve = new WCNurbsCurve(3, &cpCol, WCNurbsMode::Default());	
	std::cout << curve << std::endl;
	delete kpCol;
	
	/*** Test 2 ***/
	
	WPFloat data[] = {
		//First row
		0.0,	0.0,		0.0, 1.0, 			
		0.0,	1.0,		0.0, 0.707106781187,
		1.0,	1.0,		0.0, 1.0,
	};	
	WCCollection<WCControlPoint*> *cpCol = WCControlPoint::CollectionFromArray(3, data);
	curve = new WCNurbsCurve(2, cpCol, WCNurbsMode::Default());
	std::cout << curve << std::endl;
	WCNurbsCurve* c2 = new WCNurbsCurve(*curve);
	delete curve;
	curve = c2;
	curve->LevelOfDetail(3);
	std::cout << curve << std::endl;
	delete cpCol;
	
	/*** End Tests ***/
}


void Display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, options[OPTION_DISPLAY_WIREFRAME] ? GL_LINE : GL_FILL);
 	
    //Wireframe control
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	/*** Frame Rate Counter ***/

	static int frameCounter = 0;
	static AbsoluteTime  start;
	static WPFloat fps = 0.0;	
	char str[12];
	sprintf(str, "%4.2f fps", fps);
	glRasterPos2f(-1.8, -1.8);
	glColor4f(1.0, 1.0, 1.0, 1.0);
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

	//Render the surface
	curve->Render(0);
	if (options[OPTION_TANGENT]) {
		WCRay tangent = curve->Tangent(_tangentVal);
		tangent.Render(0);
	}
	if (options[OPTION_INVERSION]) {
		WCVector4 inv = curve->PointInversion(*_ipt);
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
//	manipulator.idle();
	glutPostRedisplay();
}


void KeyPress(unsigned char k, int x, int y) {
    k = tolower(k);
    if (optionKeyMap.find(k) != optionKeyMap.end())
        options[optionKeyMap[k]] = !options[optionKeyMap[k]];

    switch(k) {
		case 'c': curve->DrawControlPoints(!curve->DrawControlPoints()); break;
		//LOD Stuff
		case '=': curve->LevelOfDetail(curve->LevelOfDetail() + 1); break;
		case '-': curve->LevelOfDetail(curve->LevelOfDetail() - 1); break;		
		//Degree Stuff
		case ']': curve->Degree(curve->Degree() + 1); break;
		case '[': curve->Degree(curve->Degree() - 1); break;
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
		case 'k': curve->InsertKnot(2.5, 1); break;	
	}
	//Force GLUT to redraw
	glutPostRedisplay();
}


void Resize(int w, int h) {
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
		pt = (WCControlPoint*)curve->HitTest(hitTest, 0.03 );
		if (pt != NULL) pt->IsSelected(true);
		else
			manipulator.mouse(button, state, x, y);	
	}
	//Force GLUT to redraw
	glutPostRedisplay();
	
}


void MouseMove(int x, int y) {
	if (options[OPTION_ANIMATE])
		manipulator.motion(x, y);
}


void PassiveMouseMove(int x, int y) {
	if (!options[OPTION_ANIMATE]) {		
	}
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
	optionKeyMap['s'] = OPTION_DISPLAY_WIREFRAME;
	options[OPTION_DISPLAY_WIREFRAME] = false;
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