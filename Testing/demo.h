#ifndef __NURBS_H__
#define __NURBS_H__



//Standard include files
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <map>
//GL related include files
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <GLUT/glut.h>

#define WC_DEBUG


/********************************************************************************************************************/


//Function Declarations
void InitOpenGL(void);
void InitNurbs(void);
void Display(void);
void Idle(void);
void KeyPress(unsigned char k, int x, int y);
void Resize(int w, int h);
void MousePress(int button, int state, int x, int y);
void MouseMove(int x, int y);


/********************************************************************************************************************/


#endif //__NURBS_H__

