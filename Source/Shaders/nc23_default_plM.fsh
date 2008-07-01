//extension GL_ARB_texture_rectangle : require

//Uniform Textures
uniform sampler2DRect				knotPoints;
#define kp(index)					(texture2DRect(knotPoints, vec2(index)).r)
uniform sampler2DRect				controlPoints;
#define cp(index)					(texture2DRect(controlPoints, vec2(index)))
uniform sampler2DRect				verts;

//Uniform Inputs
uniform ivec4						numParams; // { degree, cp, kp, foo }
vec4 bv;


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
	vec4 left;
	vec4 right;

	//Basis[0] is always 1.0
	bv[0] = 1.0;
	//Calculate basis values
	for (jr.x=1; jr.x<=numParams.x; jr.x++) {
		left[jr.x] = u - kp(span+1-jr.x);
		right[jr.x] = kp(span+jr.x) - u;
		savedTemp.x = 0.0;
		for (jr.y=0; jr.y<jr.x; jr.y++) {
			savedTemp.y = bv[jr.y] / (right[jr.y+1] + left[jr.x-jr.y]);	
			bv[jr.y] = savedTemp.x + right[jr.y+1] * savedTemp.y;
			savedTemp.x = left[jr.x-jr.y] * savedTemp.y;
		}
		bv[jr.x] = savedTemp.x;
	}
}


/*************************************************************************/


void main(void) {
	int i, j, span, index;
	vec3 pos;
	float w;
	
	//Get basic vertex info
	vec4 inVert = texture2DRect(verts, floor(gl_FragCoord.xy));
	//Find the span for the vertex
	span = FindSpan(inVert.x);
	BasisValues(inVert.x, span);
	
	//Make sure to zero the results
	pos = vec3(0.0);
	w = 0.0;
	//Loop through each basis value
	for (j=0; j<=numParams.x; j++) {
		index = span - numParams.x + j;
		w = w + cp(index).a * bv[j];
		pos = pos + cp(index).rgb * cp(index).a * bv[j];
	}
	//Set the position (don't forget the w divide) and write out
	pos = pos / w;
	gl_FragColor = vec4(pos, 1.0);			
}


/*************************************************************************/

