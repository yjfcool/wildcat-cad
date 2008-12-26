uniform vec3 panZoomFactor;						// [ panX, panY, zoom ]


void main(void) {
	vec3 lightPos = vec3(0.0, 0.0, panZoomFactor[2] * 2.0);
	vec3 skyColor = vec3(1.0, 1.0, 1.0);
	vec3 groundColor = vec3(0.0, 0.0, 0.0);
	
	gl_Position = ftransform();
	vec3 ecPosition = gl_Position.xyz;
	vec3 tnorm = normalize(gl_NormalMatrix * gl_Normal);
	vec3 lightVec = normalize(lightPos - ecPosition);
	float costheta = dot(tnorm, lightVec);
	float a = 0.5 + 0.5 * costheta;
	gl_FrontColor = vec4(mix(groundColor, skyColor, a), 1.0);
}

