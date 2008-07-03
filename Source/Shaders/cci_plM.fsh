//extension GL_ARB_texture_rectangle : require

//Uniform Textures
uniform sampler2DRect				leftVerts;
uniform sampler2DRect				rightVerts;


//Uniform Inputs
uniform vec2						params;				// {lod, tol}


/*************************************************************************/


void main(void) {
	int i = 0;
	int lod = int(params.x);
	float dist, minDist = params.y;
	
	//Get basic vertex info
	vec4 leftVert = texture2DRect(leftVerts, floor(gl_FragCoord.xy));
	vec4 rightVert;
	gl_FragColor = vec4(-1.0, -1.0, -1.0, -1.0);
	
	//Loop through each right vertex value
	while (i < lod) {
		//Get right vertex
		rightVert = texture2DRect(rightVerts, vec2(i));
		//Determine distance from inVert to 
		dist = distance(leftVert, rightVert);
		//If distance is within tolerance, set the return
		if (dist <= params.y) {
			//Make sure we are continuing to reduce minDist
			if (dist < minDist) {
				minDist = dist;
				//Set the parametric v value
				gl_FragColor = vec4(i, leftVert.xyz);
			}
			//Otherwise we are done here
			else i = lod;
		}
		//Just increment i
		i++;
	}
}


/*************************************************************************/

