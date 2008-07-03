// Uniform Variables 
uniform vec3 panZoomFactor;						// [ panX, panY, zoom ]

// Varying Variables
varying float NdotL;
varying vec3 ReflectVec;
varying vec3 ViewVec;
varying vec3 SurfaceColor;


void main(void) {
	//Local Variables
	vec3 LightPosition = vec3(0.0, 0.0, 2.0 * panZoomFactor[2]);

	gl_Position		= ftransform();
	vec3 ecPos		= gl_Position.xyz;
	vec3 tnorm		= normalize(gl_NormalMatrix * gl_Normal);
	vec3 lightVec	= normalize(LightPosition - ecPos);
	ReflectVec		= normalize(reflect(-lightVec, tnorm));
	ViewVec			= normalize(-ecPos);
	NdotL			= (dot(lightVec, tnorm) + 1.0) * 0.5;
	SurfaceColor	= gl_Color.xyz;
}