//extension GL_ARB_texture_rectangle : require

//Uniform Textures
uniform sampler2DRect				leftVerts;


//Uniform Inputs
uniform vec2						params;					// {lod, tol}
uniform vec4						lineBegin, lineEnd;		// - pretty self explanitory


/*************************************************************************/


void main(void) {
	int lod = int(params.x);
	float dist;
	gl_FragColor = vec4(-1.0);
	
	//Get index of this point
	vec2 addr = floor(gl_FragCoord.xy);
	//Get the start of the curve line segment
	vec4 p1 = texture2DRect(leftVerts, addr);
	//Get the address of the next point
	addr.x += 1.0;
	//See if this is last vertex
	if (addr.x < params.x) {
		//Get the end of the curve line segment
		vec4 p2 = texture2DRect(leftVerts, addr);

		//Get line direction vectors
		vec4 p21 = p2 - p1;
		vec4 p43 = lineEnd - lineBegin;
		//Cross the lines to see if parallel
		vec3 crossProd = cross(p43.xyz, p21.xyz);
		float denom = length(crossProd);
		//See if lines are parallel
		if (denom < 0.001) {
			//Parallel path
			gl_FragColor = vec4(-1.0, 999.0, 999.0, 999.0);
		}
		//Non-parallel path
		else {
			//Many dot products
			float d121 =	dot(p1, p21);			// L
			float d2121 =	dot(p21, p21);			// M
			float d321 =	dot(lineBegin, p21);		// N
			float d4321 =	dot(p43, p21);			// O
			float d143 =	dot(p1, p43);			// Q
			float d343 =	dot(lineBegin, p43);	// R
			float d4343 =	dot(p43, p43);			// S
			denom = (d4343 * d2121) - (d4321 * d4321);
			//Calculate parametric intersection values
			float numer = d4343 * (d321 - d121) + d4321 * (d143 - d343);
			float mua = numer / denom;
			float mub = (d143 + d4321 * mua - d343) / d4343;

			//Bounds check u for [0, 1] - return empty if out of bounds
			if ( (mub > -params.y) && (mub < (1.0 + params.y)) ) {
				//Bound u [0.0 to 1.0]
				mua = max( min(mua, 1.0), 0.0);
				mub = max( min(mub, 1.0), 0.0);
				gl_FragColor.y = mub;
				//Get the point on the line
				vec4 ptOnLeft = p1 + p21 * mua;
				vec4 ptOnRight = lineBegin + p43 * mub;
				//Get distance from point on line to point
				dist = distance(ptOnLeft, ptOnRight);
				gl_FragColor.z = dist;
				if (dist <= params.y) {
					//Set the output values
					gl_FragColor = vec4(mub, ptOnRight.xyz);
				}
			}
		}
	}
}


/*************************************************************************/

