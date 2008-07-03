//Uniform Textures
uniform sampler2DRect				knotPointsU;
uniform sampler2DRect				knotPointsV;
uniform sampler2DRect				controlPoints;
uniform sampler2DRect				verts;

#define cp(i,j)						(texture2DRect(controlPoints, vec2(i,j)))
#define kp(sampler, index)			(texture2DRect(sampler, vec2(index)).r)


//Uniform Inputs
uniform ivec4						numParamsU;	// { degree, cp, kp, foo }
uniform ivec4						numParamsV;	// { degree, cp, kp, foo }


/*************************************************************************/


int FindSpan(in float u, in ivec4 numParams, in sampler2DRect knotPoints) {
	int retVal, i;
	//Check the special case
	if (u >= kp(knotPoints, numParams.y)) retVal = numParams.y-1;
	//Set up search
	else {
		for (i = numParams.x; i<numParams.y; i++)
			if ( (u >= kp(knotPoints, i)) && (u < kp(knotPoints, i+1)) ) retVal = i;
	}
	return retVal;
}


/*************************************************************************/


void BasisValues(in float u, in int span, out vec4 bv, out vec4 bvD, in ivec4 numParams, in sampler2DRect knotPoints) {
	ivec3 jr;
	vec2 savedTemp;
	vec4 left;
	vec4 right;

	//Basis[0] is always 1.0
	bv[0] = 1.0;
	//Calculate basis values
	for (jr.x=1; jr.x<=numParams.x; jr.x++) {
		left[jr.x] = u - kp(knotPoints, span+1-jr.x);
		right[jr.x] = kp(knotPoints, span+jr.x) - u;
		savedTemp.x = 0.0;
		for (jr.y=0; jr.y<jr.x; jr.y++) {
			savedTemp.y = bv[jr.y] / (right[jr.y+1] + left[jr.x-jr.y]);
			bv[jr.y] = savedTemp.x + right[jr.y+1] * savedTemp.y;
			savedTemp.x = left[jr.x-jr.y] * savedTemp.y;
		}
		bv[jr.x] = savedTemp.x;
		
		//Calculate N' if on second to last iteration
		if (jr.x == numParams.x-1) {
			savedTemp.x = 0.0;
			//Loop through all basis values
			for (jr.y=0; jr.y<numParams.x; jr.y++) {
				//Calculate a temp variable
				jr.z = span - numParams.x + jr.y + 1;
				//Calculate right side
				savedTemp.y = (float(numParams.x) * bv[jr.y]) / (kp(knotPoints, jr.z + numParams.x) - kp(knotPoints, jr.z));
				//Calculate derivative value
				bvD[jr.y] =  savedTemp.x - savedTemp.y;
				//Swap right side to left
				savedTemp.x = savedTemp.y;
			}
			//Save the last der-basis
			bvD[numParams.x] = savedTemp.x;
		}
	}
}


/*************************************************************************/


void main() {
	int i, j, index;
	ivec2 span;
	vec2 inPos = floor(gl_FragCoord.xy);
	vec3 pos, derX, derY;
	vec4 bvU, bvV, bvUU, bvVV;

	//Get basic vertex info
	vec4 inVert = texture2DRect(verts, inPos);
	//Find the span for the vertex
	span[0] = FindSpan(inVert[0], numParamsU, knotPointsU);
	span[1] = FindSpan(inVert[1], numParamsV, knotPointsV);
	BasisValues(inVert[0], span[0], bvU, bvUU, numParamsU, knotPointsU);
	BasisValues(inVert[1], span[1], bvV, bvVV, numParamsV, knotPointsV);

	//Make sure to zero the results
	pos = vec3(0.0);
	derX = vec3(0.0);
	derY = vec3(0.0);
	//Loop through each basis value
	for (i=0; i<=numParamsV.x; i++) {
		for (j=0; j<=numParamsU.x; j++) {
			pos += (cp(span.x - numParamsU.x + j,span.y - numParamsV.x + i).xyz * bvU[j] * bvV[i]);
			derX += (cp(span.x - numParamsU.x + j,span.y - numParamsV.x + i).xyz * bvUU[j] * bvV[i]);
			derY += (cp(span.x - numParamsU.x + j,span.y - numParamsV.x + i).xyz * bvU[j] * bvVV[i]);
		}
	}
	//Set the position (don't forget the w divide) and emit
//	pos = pos / w;
	gl_FragData[0] = vec4(pos, 1.0);	//Calculate the normal
	
	//Write normal information
	vec4 norm = normalize(vec4(cross(derX, derY), 0.0));
	gl_FragData[1] = norm;
}
