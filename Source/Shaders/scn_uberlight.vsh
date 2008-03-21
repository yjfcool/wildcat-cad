uniform vec4 ViewPosition;
uniform mat4 WCtoLC;
uniform mat4 WCtoLCit;
uniform mat4 MCtoWC;
uniform mat4 MCtoWCit;

varying vec3 LCpos;
varying vec3 LCnorm;
varying vec3 LCcamera;


/*************************************************************************/


void main(void) {
	gl_Position = ftransform();
	
	//Compute world space position and normal
	vec4 wcPos = MCtoWC * gl_Vertex;
	vec3 wcNorm = (MCtoWCit * vec4(gl_Normal, 0.0)).xyz;
	
	//Compute light coordinate system camera position,
	// vertex position and normal
	LCcamera = (WCtoLC * ViewPosition).xyz;
	LCpos = (WCtoLC * wcPos).xyz;
	LCnorm = (WCtoLCit * vec4(wcNorm, 0.0)).xyz;
}


/*************************************************************************/

