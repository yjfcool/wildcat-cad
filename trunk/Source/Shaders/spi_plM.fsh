//extension GL_ARB_texture_rectangle : require

//Uniform Textures
uniform sampler2DRect				leftVerts;


//Uniform Inputs
uniform vec2						params;					// {lod, tol}
uniform vec4						point;					// - pretty self explanitory


/*************************************************************************/


void main(void) {
	int lod = int(params.x);
	float dist, minDist = params.y;
	gl_FragColor = vec4(-1, -1.0, -1.0, -1.0);
	//Get basic vertex info
	vec4 vert = texture2DRect(leftVerts, floor(gl_FragCoord.xy));

	
}


/*************************************************************************/

