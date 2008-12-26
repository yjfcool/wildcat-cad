uniform vec3 panZoomFactor;						// [ panX, panY, zoom ]


void main(void) {
	vec3 lightPos = vec3(0.0, 0.0, panZoomFactor[2] * 2.0);
	//Compute the clip-space position
	gl_Position = ftransform();
	//Use intensity to calculate color
	gl_FrontColor = gl_Color;
	//Pass through the tex coords
	gl_TexCoord[0] = gl_MultiTexCoord0;
}