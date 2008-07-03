//Required Extensions
#extension GL_EXT_gpu_shader4: require
#extension GL_EXT_bindable_uniform: require


//Local Definitions
#define MAX_DEGREE					8
#define MAX_CONTROLPOINTS			64
#define MAX_KNOTPOINTS				64


//Uniform Inputs
uniform ivec4 numParams;			// { degree, cp, kp, numVertToGenerate }


//Bindable Uniform Inputs
bindable uniform vec4	controlPoints[MAX_CONTROLPOINTS];
bindable uniform float	knotPoints[MAX_KNOTPOINTS];


//Additional Inputs
vec4 fParams = gl_PositionIn[0];


/*************************************************************************/


mat4 coef2 = mat4(1.0, -2.0, 1.0, 0.0,						// Degree 2
				  -2.0, 2.0, 0.0, 0.0, 
				  1.0, 0.0, 0.0, 0.0,
				  0.0, 0.0, 0.0, 0.0);
mat4 coef3 = mat4(-1.0, 3.0, -3.0, 1.0,						// Degree 3
				  3.0, -6.0, 3.0, 0.0, 
				  -3.0, 3.0, 0.0, 0.0, 
				  1.0, 0.0, 0.0, 0.0);						


/*************************************************************************/


vec4 CalcDegree2(float u) {
	mat4 pts = mat4(controlPoints[0].x, controlPoints[1].x, controlPoints[2].x, 0.0,
				controlPoints[0].y, controlPoints[1].y, controlPoints[2].y, 0.0,
				controlPoints[0].z, controlPoints[1].z, controlPoints[2].z, 0.0,
				controlPoints[0].w, controlPoints[1].w, controlPoints[2].w, 0.0);
	//Calculate the point
	vec4 t = vec4(u*u, u, 1.0, 0.0);	
	vec4 c = t * coef2 * pts;
	return c;
}
		

/*************************************************************************/


vec4 CalcDegree3(float u) {
	mat4 pts = mat4(controlPoints[0].x, controlPoints[1].x, controlPoints[2].x, controlPoints[3].x,
				controlPoints[0].y, controlPoints[1].y, controlPoints[2].y, controlPoints[3].y,
				controlPoints[0].z, controlPoints[1].z, controlPoints[2].z, controlPoints[3].z,
				controlPoints[0].w, controlPoints[1].w, controlPoints[2].w, controlPoints[3].w);
	//Calculate the point
	float u2 = u*u;
	float u3 = u2*u;
	vec4 t = vec4(u3, u2, u, 1.0);	
	vec4 c =t * coef3 * pts;
	return c;
}


/*************************************************************************/


void main(void) {
	int i;
	float u = fParams.x;	
	vec4 pos;

	//Loop for each vertex in the segment
	for (i=0; i<numParams.w; i++) {
		//Get the value
		if (numParams.x == 2)	
			pos = CalcDegree2(u);
		else
			pos = CalcDegree3(u);
		//Set the position (don't forget the w divide) and emit
		gl_Position = vec4(pos.xyz , 1.0);
		EmitVertex();
		//Increment u, but make sure not to exceed (floating point errors)
		u = min(u+fParams.y, fParams.z);
	}
	//Not necessary
	EndPrimitive();
}


/*************************************************************************/

