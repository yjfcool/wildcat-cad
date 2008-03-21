//Required Extensions
#extension GL_EXT_gpu_shader4: require
#extension GL_EXT_bindable_uniform: require


//Local Definitions
#define MAX_DEGREE					7
#define MAX_CONTROLPOINTS			255
#define MAX_KNOTPOINTS				255


//Uniform Inputs
uniform ivec4 numParamsU;			// { degreeU, cpU, kpU, --- }
uniform ivec4 numParamsV;			// { degreeV, cpV, kpV, --- }
varying out vec4 nurbNormal;		// Output of the vertex normal
varying out vec2 nurbTex;			// Output of the vertex texture coordinate


//Bindable Uniform Inputs
bindable uniform vec4	controlPoints[MAX_CONTROLPOINTS];
bindable uniform float	knotPointsU[MAX_KNOTPOINTS];
bindable uniform float	knotPointsV[MAX_KNOTPOINTS];


//Additional Inputs
vec4 fParams = gl_PositionIn[0];	// { u, v, ---, ---}
float basisValuesU[MAX_DEGREE*2];
float basisValuesV[MAX_DEGREE*2];


/*************************************************************************/


int FindSpanU(float u) {
	int retVal;
	//Check the special case
	if ( u >= knotPointsU[numParamsU.y] ) {
		retVal = numParamsU.y-1;
	}
	else {
		//Set up search
		ivec3 lmh = ivec3(numParamsU.x, 0, numParamsU.y);
		lmh.y = (lmh.x + lmh.z) / 2;
		//Start binary search
		while ((u < knotPointsU[lmh.y]) || (u >= knotPointsU[lmh.y+1])) {  
			if (u < knotPointsU[lmh.y]) lmh.z = lmh.y;
			else lmh.x = lmh.y;
			lmh.y = (lmh.x + lmh.z) / 2;
		}
		//Return the span value
		retVal = lmh.y;
	}
	return retVal;
}


int FindSpanV(float v) {
	int retVal;
	//Check the special case
	if ( v >= knotPointsV[numParamsV.y] ) {
		retVal = numParamsV.y-1;
	}
	else {
		//Set up search
		ivec3 lmh = ivec3(numParamsV.x, 0, numParamsV.y);
		lmh.y = (lmh.x + lmh.z) / 2;
		//Start binary search
		while ((v < knotPointsV[lmh.y]) || (v >= knotPointsV[lmh.y+1])) {  
			if (v < knotPointsV[lmh.y]) lmh.z = lmh.y;
			else lmh.x = lmh.y;
			lmh.y = (lmh.x + lmh.z) / 2;
		}
		//Return the span value
		retVal = lmh.y;
	}
	return retVal;
}


/*************************************************************************/


void BasisValuesU(float u, int span) {
	ivec3 jr;
	vec2 savedTemp;
	vec2 leftRight[MAX_DEGREE];

	//Basis[0] is always 1.0
	basisValuesU[0] = 1.0;
	//Calculate basis values
	for (jr.x=1; jr.x<=numParamsU.x; jr.x++) {
		leftRight[jr.x].x = u - knotPointsU[span+1-jr.x];
		leftRight[jr.x].y = knotPointsU[span+jr.x] - u;
		savedTemp.x = 0.0;
		for (jr.y=0; jr.y<jr.x; jr.y++) {
			savedTemp.y = basisValuesU[jr.y] / (leftRight[jr.y+1].y + leftRight[jr.x-jr.y].x);
			basisValuesU[jr.y] = savedTemp.x + leftRight[jr.y+1].y * savedTemp.y;
			savedTemp.x = leftRight[jr.x-jr.y].x * savedTemp.y;
		}
		basisValuesU[jr.x] = savedTemp.x;
		
		//Calculate N' if on second to last iteration
		if (jr.x == numParamsU.x-1) {
			savedTemp.x = 0.0;
			//Loop through all basis values
			for (jr.y=0; jr.y<numParamsU.x; jr.y++) {
				//Calculate a temp variable
				jr.z = span - numParamsU.x + jr.y + 1;
				//Calculate right side
				savedTemp.y = (float(numParamsU.x) * basisValuesU[jr.y]) / (knotPointsU[jr.z + numParamsU.x] - knotPointsU[jr.z]);
				//Calculate derivative value
				basisValuesU[numParamsU.x + jr.y + 1] =  savedTemp.x - savedTemp.y;
				//Swap right side to left
				savedTemp.x = savedTemp.y;	
			}
			//Save the last der-basis
			basisValuesU[numParamsU.x * 2 + 1] = savedTemp.x;
		}
	}
}


void BasisValuesV(float v, int span) {
	ivec3 jr;
	vec2 savedTemp;
	vec3 leftRight[MAX_DEGREE];				//<-- want to use: leftRight[numParams.x];

	//Basis[0] is always 1.0
	basisValuesV[0] = 1.0;
	//Calculate basis values
	for (jr.x=1; jr.x<=numParamsV.x; jr.x++) {
		leftRight[jr.x].x = v - knotPointsV[span+1-jr.x];
		leftRight[jr.x].y = knotPointsV[span+jr.x] - v;
		savedTemp.x = 0.0;
		for (jr.y=0; jr.y<jr.x; jr.y++) {
			savedTemp.y = basisValuesV[jr.y] / (leftRight[jr.y+1].y + leftRight[jr.x-jr.y].x);
			basisValuesV[jr.y] = savedTemp.x + leftRight[jr.y+1].y * savedTemp.y;
			savedTemp.x = leftRight[jr.x-jr.y].x * savedTemp.y;
		}
		basisValuesV[jr.x] = savedTemp.x;

		//Calculate N' if on second to last iteration
		if (jr.x == numParamsV.x-1) {
			savedTemp.x = 0.0;
			//Loop through all basis values
			for (jr.y=0; jr.y<numParamsV.x; jr.y++) {
				//Calculate a temp variable
				jr.z = span - numParamsV.x + jr.y + 1;
				//Calculate right side
				savedTemp.y = (float(numParamsV.x) * basisValuesV[jr.y]) / (knotPointsV[jr.z + numParamsV.x] - knotPointsV[jr.z]);
				//Calculate derivative value
				basisValuesV[numParamsV.x + jr.y + 1] =  savedTemp.x - savedTemp.y;
				//Swap right side to left
				savedTemp.x = savedTemp.y;	
			}
			//Save the last der-basis
			basisValuesV[numParamsU.x * 2 + 1] = savedTemp.x;
		}
	}
}


/*************************************************************************/


void main(void) {
	int i, j, index;
	ivec2 span;
	vec3 pos, derX, derY;
	float w;

	//Find the span for each direction
	span.x = FindSpanU(fParams.x);
	span.y = FindSpanV(fParams.y);
	//Find the basis vector for each direction
	BasisValuesU(fParams.x, span.x);
	BasisValuesV(fParams.y, span.y);
	
	//Make sure to zero the results
	pos = vec3(0.0);
	derX = vec3(0.0);
	derY = vec3(0.0);
	w = 0.0;
	//Loop through each basis value
	for (i=0; i<=numParamsV.x; i++) {
		index = (numParamsU.y * (span.y - numParamsV.x + i)) + span.x - numParamsU.x;
		for (j=0; j<=numParamsU.x; j++) {
			w   = w   + (controlPoints[index].w * basisValuesU[j] * basisValuesV[i]);
			pos = pos + (controlPoints[index].xyz * controlPoints[index].w * basisValuesU[j] * basisValuesV[i]);
			derX = derX + (controlPoints[index].xyz * basisValuesU[numParamsU.x+1+i] * basisValuesV[j]);
			derY = derY + (controlPoints[index].xyz * basisValuesU[i] * basisValuesV[numParamsV.x+1+j]);
			index++;
		}
	}
	//Set the position (don't forget the w divide) and emit
	pos = pos / w;
	gl_Position = vec4(pos, 1.0);
//	gl_Position = vec4(fParams.xy, span.xy);
//	gl_Position = vec4(1.0, 2.0, 3.0, 4.0);	
	//Calculate the normal
	nurbNormal = vec4( normalize(cross(derY, derX)), 0.0);
	//Tex Coordinates are just teh u and v values
	nurbTex = vec2(fParams.xy);
	//Emit the vertex
	EmitVertex();
}

/*************************************************************************/