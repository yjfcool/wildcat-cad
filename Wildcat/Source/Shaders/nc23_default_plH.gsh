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
float basisValues[MAX_DEGREE];


/*************************************************************************/


int FindSpan(float u) {
	//Check the special case
	if (u >= knotPoints[numParams.y]) return numParams.y-1;
	
	//Set up search
	ivec3 lmh = ivec3(numParams.x, 0, numParams.y);
	lmh.y = (lmh.x + lmh.z) / 2;
	//Start binary search
	while ((u < knotPoints[lmh.y]) || (u >= knotPoints[lmh.y+1])) {  
		if (u < knotPoints[lmh.y]) lmh.z = lmh.y;
		else lmh.x = lmh.y;
		lmh.y = (lmh.x + lmh.z) / 2;
	}
	//Return the span value
	return lmh.y;
}


/*************************************************************************/


void BasisValues(float u, int span) {
	ivec2 jr;
	vec2 savedTemp;
	vec2 leftRight[MAX_DEGREE];				//<-- want to use: leftRight[numParams.x];

	//Basis[0] is always 1.0
	basisValues[0] = 1.0;
	//Calculate basis values
	for (jr.x=1; jr.x<=numParams.x; jr.x++) {
		leftRight[jr.x].x = u - knotPoints[span+1-jr.x];
		leftRight[jr.x].y = knotPoints[span+jr.x] - u;
		savedTemp.x = 0.0;
		for (jr.y=0; jr.y<jr.x; jr.y++) {
			savedTemp.y = basisValues[jr.y] / (leftRight[jr.y+1].y + leftRight[jr.x-jr.y].x);
			basisValues[jr.y] = savedTemp.x + leftRight[jr.y+1].y * savedTemp.y;
			savedTemp.x = leftRight[jr.x-jr.y].x * savedTemp.y;
		}
		basisValues[jr.x] = savedTemp.x;
	}
}


/*************************************************************************/


void main(void) {
	int i, j, span, index;
	float u = fParams.x;	
	vec3 pos;
	float w;

	//Loop for each vertex in the segment
	for (i=0; i<numParams.w; i++) {
		//Find the span and basis values
		span = FindSpan(u);
		BasisValues(u, span);
		//Make sure to zero the results
		pos = vec3(0.0);
		w = 0.0;
		//Loop through each basis value
		for (j=0; j<=numParams.x; j++) {
			index = span - numParams.x + j;
			w += (controlPoints[index].w * basisValues[j]);
			pos += controlPoints[index].xyz * controlPoints[index].w * basisValues[j];
		}
		//Set the position (don't forget the w divide) and emit
		gl_Position = vec4(pos / w, 1.0);
		EmitVertex();
		//Increment u, but make sure not to exceed (floating point errors)
		u = min(u+fParams.y, fParams.z);
	}
	//Not necessary
	EndPrimitive();
}
