uniform vec3 panZoomFactor;						// [ panX, panY, zoom ]


void main(void) {
	vec3 lightPos = vec3(0.0, 0.0, panZoomFactor[2] * 2.0);

	//Compute the clip-space position
	gl_Position = ftransform();
	vec3 norm = normalize(gl_NormalMatrix * gl_Normal);
		
	//Compute light direction
	vec3 lightDir = normalize(lightPos - gl_Position.xyz);
		
	//Calculate light intensity
	float intensity = dot(norm, lightDir);
	
	//Use intensity to calculate color
	gl_FrontColor = vec4(gl_Color.rgb * intensity, gl_Color.a);
	
	//Pass through the tex coords
	gl_TexCoord[0] = gl_MultiTexCoord0;
}