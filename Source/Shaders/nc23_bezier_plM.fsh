//extension GL_ARB_shading_language_120 : require

//Uniform Textures
uniform sampler2DRect				knotPoints;
#define kp(index)					(texture2DRect(knotPoints, vec2(index)).r)
uniform sampler2DRect				controlPoints;
#define cp(index)					(texture2DRect(controlPoints, vec2(index)))

//Uniform Inputs
uniform ivec4						numParams;			// { degree, cp, texWidth, texHeight }
uniform vec2						fltParams;			// { start, step }


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
	mat4 pts = mat4(cp(0).r, cp(1).r, cp(2).r, 0.0,
					cp(0).g, cp(1).g, cp(2).g, 0.0,
					cp(0).b, cp(1).b, cp(2).b, 0.0,
					cp(0).a, cp(1).a, cp(2).a, 0.0);
	//Calculate the point
	vec4 t = vec4(u*u, u, 1.0, 0.0);	
	vec4 c = t * coef2 * pts;
	return c;
}
		

/*************************************************************************/


vec4 CalcDegree3(float u) {
	mat4 pts = mat4(cp(0).r, cp(1).r, cp(2).r, cp(3).r,
					cp(0).g, cp(1).g, cp(2).g, cp(3).g, 
					cp(0).b, cp(1).b, cp(2).b, cp(3).b, 
					cp(0).a, cp(1).a, cp(2).a, cp(3).a);
	//Calculate the point
	float u2 = u*u;
	float u3 = u2*u;
	vec4 t = vec4(u3, u2, u, 1.0);	
	vec4 c =t * coef3 * pts;
	return c;
}


/*************************************************************************/


void main(void) {
	//Calculate u value
	vec2 inPos = floor(gl_FragCoord.xy);
	float u = fltParams.x + inPos.x * fltParams.y;

	//Calculate the output value
	if (numParams.x == 2)	gl_FragColor = CalcDegree2(u);
	else					gl_FragColor = CalcDegree3(u);
}


/*************************************************************************/

