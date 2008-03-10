#include "demo.h"
#include "adapter.h"
#include "collection.h"
#include "control_point.h"
#include "nurbs_curve.h"
#include "curve_profile.h"
#include "trimmed_nurbs_surface.h"


#include <CoreServices/CoreServices.h>


WCNurbsSurface *surface;
WCCurveProfile *profile;
WCNurbsCurve *c1, *c2, *c3, *cX;
WCVector4 mark, move, spot;
WCControlPoint *pt;
GLuint prog;


/********************************************************************************************************************/


void InitOpenGL(void) {
	WCAdapter::Initialize();
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2, 0.2, 0.2, 1.0);

	options[OPTION_ENABLE_LIGHTING] = false;
	options[OPTION_DISPLAY_WIREFRAME] = true;
	
	srand ( time(NULL) );
}


void InitShaders(void) {
	GLuint xform_list = CompileGLSLShaderFromFile(GL_VERTEX_SHADER,		"../../shaders/xform_light.vsh");
	//First create a program object
	prog = glCreateProgram();
	//Attach vertex shader
	glAttachShader(prog, xform_list);
	//Link the program
	glLinkProgram(prog);
	GLint linked;
	glGetProgramiv(prog, GL_OBJECT_LINK_STATUS_ARB, &linked);
	if (!linked) {
		char temp[256] = "";
		//If not, print out compilation errors
		glGetProgramInfoLog(prog, 256, NULL, temp);
		std::cout << "\tDemo Program link failed:\n" << temp << std::endl;
		return;
	}
}


void InitNurbs(void) {
/*** Base Surface Definition ***/
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
//	WCCollection<WCControlPoint*> *surfCP = WCControlPoint::CollectionFromArray(16, data);
//	pt = surfCP->At(5);
//	surface = new WCNurbsSurface(3, 3, 4, 4, surfCP, WCNurbsMode::Default(), WCNurbsMode::Default());
//	std::cout << surface << std::endl;
//	surface->LevelOfDetail(1);
//	delete surfCP;
//	surface->DrawTrimCurves(true);
	

/*** Create inside and outside curve profiles ***/
	WCCurveProfile *cpOutside = new WCCurveProfile();
	WCCurveProfile *cpInside1 = new WCCurveProfile();
	WCCurveProfile *cpInside2 = new WCCurveProfile();
//	surface->AddTrimCurve(cpOutside, false);
//	surface->AddTrimCurve(cpInside1, true);
//	surface->AddTrimCurve(cpInside2, true);


/*** Outside Curves ***/
	WCControlPoint *p0 = new WCControlPoint(0.25, 0.25, 0.0);
	WCControlPoint *p1 = new WCControlPoint(0.25, 0.75, 0.0);
	WCControlPoint *p2 = new WCControlPoint(1.50, 0.75, 0.0);
	WCControlPoint *p3 = new WCControlPoint(1.75, 0.75, 0.0, 0.707106781187);
//	WCControlPoint *p3 = new WCControlPoint(1.75, 0.75, 0.0);
	WCControlPoint *p4 = new WCControlPoint(1.75, 0.50, 0.0);
	WCControlPoint *p5 = new WCControlPoint(1.75, 0.25, 0.0);
	
	//Collection for curve 0
	WCCollection<WCControlPoint*> col;
	WCNurbsCurve *curve;	

	col.Add(p0);
	col.Add(p1);
	curve = new WCNurbsCurve(1, &col, WCNurbsMode::Default());
	c1 = curve;
	cpOutside->AddCurve(curve);
	col.Clear();
	
	//Collection for curve 1
	col.Add(p1);
	col.Add(p2);
	curve = new WCNurbsCurve(1, &col, WCNurbsMode::Default());
	cpOutside->AddCurve(curve);
	col.Clear();
	//Collection for curve 2
	col.Add(p2);
	col.Add(p3);
	col.Add(p4);
	curve = new WCNurbsCurve(2, &col, WCNurbsMode::Default());
	std::cout << curve << std::endl;
	cpOutside->AddCurve(curve);	
	col.Clear();
	//Collection for curve 3	
	col.Add(p4);
	col.Add(p5);
	curve = new WCNurbsCurve(1, &col, WCNurbsMode::Default());
	cpOutside->AddCurve(curve);	
	col.Clear();
	//Collection for curve 4
	col.Add(p5);
	col.Add(p0);
	curve = new WCNurbsCurve(1, &col, WCNurbsMode::Default());
	cpOutside->AddCurve(curve);	
	col.Clear();
	
/*** Debug ***/
	profile = cpOutside;
//	std::cout << "Length: " << profile->Length() << std::endl;
/*** Debug ***/	
					
/*** Inside Curves ***/
	p0 = new WCControlPoint(0.75, 0.33333333, 0.0);
	p1 = new WCControlPoint(0.5, 0.5, 0.0);
	p2 = new WCControlPoint(0.75, 0.66666666, 0.0);
	p3 = new WCControlPoint(1.0, 0.5, 0.0);	

	//Collection for curve 0
	col.Add(p0);
	col.Add(p1);
	col.Add(p2);
	curve = new WCNurbsCurve(2, &col, WCNurbsMode::Default());
	curve->LevelOfDetail(16);
	cpInside1->AddCurve(curve);
	col.Clear();
	//Collection for curve 1
	col.Add(p2);
	col.Add(p3);
	col.Add(p0);
	curve = new WCNurbsCurve(2, &col, WCNurbsMode::Default());
	curve->LevelOfDetail(16);
	cpInside1->AddCurve(curve);
	col.Clear();
	
	p0 = new WCControlPoint(1.25, 0.333333333, 0.0);
	p1 = new WCControlPoint(1.0, 0.5, 0.0);
	p2 = new WCControlPoint(1.25, 0.6666666, 0.0);
	p3 = new WCControlPoint(1.5, 0.6666666, 0.0);	
	p4 = new WCControlPoint(1.4, 0.6, 0.0);
	p5 = new WCControlPoint(1.6, 0.733333333, 0.0);
	WCControlPoint *p6 = new WCControlPoint(1.5, 0.33333333, 0.0);	
	
	//Collection for curve 0
	col.Add(p0);
	col.Add(p1);
	curve = new WCNurbsCurve(1, &col, WCNurbsMode::Default());
	cpInside2->AddCurve(curve);
	col.Clear();
	//Collection for curve 1
	col.Add(p1);
	col.Add(p2);
	curve = new WCNurbsCurve(1, &col, WCNurbsMode::Default());
	cpInside2->AddCurve(curve);
	col.Clear();
	//Collection for curve 2
	col.Add(p2);
	col.Add(p3);
	curve = new WCNurbsCurve(1, &col, WCNurbsMode::Default());
	cpInside2->AddCurve(curve);
	col.Clear();
	//Collection for curve 3
	col.Add(p3);
	col.Add(p4);
	col.Add(p5);
	col.Add(p6);
	curve = new WCNurbsCurve(3, &col, WCNurbsMode::Default());
	curve->LevelOfDetail(16);	
	cpInside2->AddCurve(curve);
	col.Clear();
	//Collection for curve 4
	col.Add(p6);
	col.Add(p0);
	curve = new WCNurbsCurve(1, &col, WCNurbsMode::Default());	
	cpInside2->AddCurve(curve);	
	col.Clear();
/*** ***
	c1 = new WCNurbsCurve(WCNurbsMode::Default(), 1, col);
	c2 = new WCNurbsCurve(WCNurbsMode::Default(), 2, col);
	c3 = new WCNurbsCurve(WCNurbsMode::Default(), 3, col);
	cX = new WCNurbsCurve(WCNurbsMode::Default(), 1, col);
/*** Debug ***/	
}


void Display(void) {
	static int frameCounter = 0;
	static AbsoluteTime  start;
  
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, options[OPTION_DISPLAY_WIREFRAME] ? GL_LINE : GL_FILL);
 	
    //Wireframe control
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	//Timer stuff
	if (frameCounter == 0) {
		start = UpTime();	
	}
	if (frameCounter == 100) {
		Nanoseconds elapsed = AbsoluteDeltaToNanoseconds(UpTime(), start);
		WPFloat fps = 100.0 / UnsignedWideToUInt64(elapsed) * 1000000000;		
		int len;
		char str[] = "Counter";
		glRasterPos2f(0.5, 0.5);
		glColor4f(1.0, 1.0, 1.0, 1.0);
		len = (int) strlen(str);
		for (int i = 0; i < len; i++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
		}
		std::cout << "FPS: " << fps << "\n";   
		frameCounter = 0;
	}
	else {
		frameCounter++;
	}
	
	//Time to dance
	WPFloat pert;
	if(!options[OPTION_ANIMATE]) {
		WCCollection<WCControlPoint*> *pts = surface->ControlPoints();
		for (int i=0; i<16; i++) {
			pert = (rand() % 100) / 500.0 - 0.1;
//			std::cout << pert << std::endl;
			pts->At(i)->Z( pts->At(i)->Z() + pert );
		}
	}
		
	
	//Now go to viewing perspeective
	manipulator.applyTransform();

	//Render the surface
//	surface->Render();
	profile->Render();
//	c1->Render();
//	c2->Render();
//	c3->Render();
//	cX->Render();

	//Swap buffers and start over
	glutSwapBuffers();
	glutReportErrors();
}


void Idle(void) {
 //   if (options[OPTION_ANIMATE]) {
        manipulator.idle();
        glutPostRedisplay();
 //   }
}


void KeyPress(unsigned char k, int x, int y) {
    k = tolower(k);
    if (optionKeyMap.find(k) != optionKeyMap.end())
        options[optionKeyMap[k]] = !options[optionKeyMap[k]];

    switch(k) {
		case 27: case 'q': exit(0);
		case 's':
			if (WCAdapter::PolygonMode() == GL_FILL) WCAdapter::PolygonMode(GL_LINE);
			else WCAdapter::PolygonMode(GL_FILL);
			break;
		case '+': case '=':
//			curve->LevelOfDetail(curve->LevelOfDetail() + 1);
			surface->LevelOfDetail(surface->LevelOfDetail() + 1);
			break;
		case '-': 
//			curve->LevelOfDetail(curve->LevelOfDetail() - 1);
			surface->LevelOfDetail(surface->LevelOfDetail() - 1);
			break;
		case 'c':
//			curve->DrawControlPoints(!curve->DrawControlPoints());
			surface->DrawControlPoints(!surface->DrawControlPoints());			
			break;
		case ']':
//			curve->Degree(curve->Degree() + 1);
//			surface->Degree(surface->DegreeU() + 1, surface->DegreeV() + 1);
			break;
		case '[':
//			curve->Degree(curve->Degree() - 1);
//			surface->Degree(surface->DegreeU() - 1, surface->DegreeV() - 1);
			break;
		case 'z':
			//Move down the z axis
			if (pt != NULL) pt->Z( pt->Z() - 0.1 );
			break;
		case 'e':
			//Move up the z axis
			if (pt != NULL) pt->Z( pt->Z() + 0.1 );
			break;
		case 'a':
			//Move down the z axis
			if (pt != NULL) pt->X( pt->X() - 0.1 );
			break;
		case 'd':
			//Move up the z axis
			if (pt != NULL) pt->X( pt->X() + 0.1 );
			break;
		case 'x':
			//Move down the z axis
			if (pt != NULL) pt->Y( pt->Y() - 0.1 );
			break;
		case 'w':
			//Move up the z axis
			if (pt != NULL) pt->Y( pt->Y() + 0.1 );
			break;			
	}
	//Force GLUT to redraw
	glutPostRedisplay();
}


void Resize(int w, int h) {
//	std::cout << "Resize: " << w << ", " << h << std::endl;
	//Set the window width and height parameters
	WCAdapter::WindowWidth(w);
	WCAdapter::WindowHeight(h);
	
	//Adjust the viewport
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat)w/(GLfloat)h, 0.01, 10.0);
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
//		pt = (WCControlPoint*)surface->HitTest(hitTest, 0.03 );
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
//		WCRay hitTest = WCRay::FromScreenCoords(x, y, 512, 512, 0.01, 10.0);	
//		spot = hitTest.Base();
//		glutPostRedisplay();		
	}
}



int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitWindowSize(512, 512);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	glutCreateWindow("NURBS Geometry Shader Rendering");

	InitOpenGL();
	InitShaders();
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
	optionKeyMap['l'] = OPTION_ENABLE_LIGHTING;
	options[OPTION_ENABLE_LIGHTING] = true;
	optionKeyMap[' '] = OPTION_ANIMATE;
	options[OPTION_ANIMATE] = true;

	//Execute GLUT main loop and exit
	glutMainLoop();
	
	//Clean up shader programs	
	glDeleteProgram(prog);	
	//Clean up surface
	//Time to exit
    return 0;
}