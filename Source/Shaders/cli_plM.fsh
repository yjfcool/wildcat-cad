//extension GL_ARB_texture_rectangle : require

//Uniform Textures
uniform sampler2DRect				leftVerts;


//Uniform Inputs
uniform vec2						params;					// {lod, tol}
uniform vec4						lineBegin, lineEnd;		// - pretty self explanitory


/*************************************************************************/


void main(void) {
	int lod = int(params.x);
	float dist, minDist = params.y;
	gl_FragColor = vec4(-1, -1.0, -1.0, -1.0);
	
	//Get basic vertex info
	vec4 leftVert = texture2DRect(leftVerts, floor(gl_FragCoord.xy));
	//Get line direction vector
	vec4 direction = lineEnd - lineBegin;
	//Get the length of the direction vector
	float dirLen = length(direction);
	//Get line-begin to point vector
	vec4 pointVector = leftVert - lineBegin;
	//Calculate the projected u value
	float u = dot(direction, pointVector) / (dirLen * dirLen);

	//Bounds check u for [0, 1] - return empty if out of bounds
	if ( (u > -params.y) && (u < (1.0 + params.y)) ) {
		//Bound u [0.0 to 1.0]
		u = max( min(u, 1.0), 0.0);
		//Get the point on the line
		vec4 pt = lineBegin + (direction * u);
		//Get distance from point on line to point
		if (distance(pt, leftVert) <= params.y) {
			//Set the output values
			gl_FragColor = vec4(u, pt.xyz);
		}
	}
}


/*************************************************************************/

