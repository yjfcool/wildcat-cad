//Uniform Textures
uniform sampler2DRect				knotPoints;
#define kp(index)					(texture2DRect(knotPoints, vec2(index)).r)
uniform sampler2DRect				controlPoints;
#define cp(index)					(texture2DRect(controlPoints, vec2(index)))

//Uniform Inputs
uniform ivec4						numParams;			// { degree, cp, texWidth, texHeight }
uniform vec2						fltParams;			// { start, step }
vec4 bva[2];
vec4 lefta[2];
vec4 righta[2];


/*************************************************************************/

float bvR(int index) {
	if (index > 4) return bva[1][index-4];
	return bva[0][index];
}

void bvW(int index, float value) {
	if (index > 4) bva[1][index-4] = value;
	else bva[0][index] = value;
}


float leftR(int index) {
	if (index > 4) return lefta[1][index-4];
	return lefta[0][index];
}

void leftW(int index, float value) {
	if (index > 4) lefta[1][index-4] = value;
	else lefta[0][index] = value;
}


float rightR(int index) {
	if (index > 4) return righta[1][index-4];
	return righta[0][index];
}

void rightW(int index, float value) {
	if (index > 4) righta[1][index-4] = value;
	else righta[0][index] = value;
}


/*************************************************************************/


int FindSpan(float u) {
	//Check the special case
	if (u >= kp(numParams.y)) return numParams.y-1;
	
	//Set up search
	ivec3 lmh = ivec3(numParams.x, 0, numParams.y);
	lmh.y = (lmh.x + lmh.z) / 2;
	//Start binary search
	int i=0;
	while ((u < kp(lmh.y)) || (u >= kp(lmh.y+1))) {  
		if (u < kp(lmh.y)) lmh.z = lmh.y;
		else lmh.x = lmh.y;
		lmh.y = (lmh.x + lmh.z) / 2;
		if (i > 4) return -1;
		i++;
	}
	//Return the span value
	return lmh.y;
}


/*************************************************************************/


void BasisValues(float u, int span) {
	ivec2 jr;
	vec2 savedTemp;

	//Basis[0] is always 1.0
	bva[0][0] = 1.0;
	//Calculate basis values
	for (jr.x=1; jr.x<=numParams.x; jr.x++) {
		leftW(jr.x, u - kp(span+1-jr.x));
		rightW(jr.x, kp(span+jr.x) - u);
		savedTemp.x = 0.0;
		for (jr.y=0; jr.y<jr.x; jr.y++) {
			savedTemp.y = bvR(jr.y) / (rightR(jr.y+1) + leftR(jr.x-jr.y));
			bvW(jr.y, savedTemp.x + rightR(jr.y+1) * savedTemp.y);
			savedTemp.x = leftR(jr.x-jr.y) * savedTemp.y;
		}
		bvW(jr.x, savedTemp.x);
	}
}


/*************************************************************************/


void main(void) {
	int j, span, index;
	
	//Calculate u value
	vec2 inPos = floor(gl_FragCoord.xy);
	float u = fltParams.x + inPos.x * fltParams.y;

	//Find the span for the vertex
	span = FindSpan(u);
	BasisValues(u, span);
	
	//Make sure to zero the results
	vec3 pos = vec3(0.0);
	float w = 0.0;
	//Loop through each basis value
	for (j=0; j<=numParams.x; j++) {
		index = span - numParams.x + j;
		w = w + cp(index).a * bvR(j);
		pos = pos + cp(index).rgb * cp(index).a * bvR(j);
	}
	//Set the position (don't forget the w divide) and write out
	pos = pos / w;
	gl_FragColor = vec4(pos, 1.0);
}


/*************************************************************************/

