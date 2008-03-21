// Uniform Variables 
uniform vec3 panZoomFactor;						// [ panX, panY, zoom ]


// Varying Variables
varying float NdotL;
varying vec3 ReflectVec;
varying vec3 ViewVec;
varying vec3 SurfaceColor;


//Local Variables
vec3 WarmColor;
vec3 CoolColor;
float DiffuseWarm;
float DiffuseCool;


void main(void) {
	//Init Variables
	WarmColor		= vec3(0.2, 0.2, 0.2);
	CoolColor		= vec3(0.0, 0.0, 1.0);
	DiffuseWarm		= 0.15;
	DiffuseCool		= 0.45;
	
	vec3 kcool		= min(CoolColor + DiffuseCool * SurfaceColor, 1.0);
	vec3 kwarm		= min(WarmColor + DiffuseWarm * SurfaceColor, 1.0);
	vec3 kfinal		= mix(kcool, kwarm, NdotL);
	
	vec3 nreflect	= normalize(ReflectVec);
	vec3 nview		= normalize(ViewVec);
	float spec		= max(dot(nreflect, nview), 0.0);
	spec			= pow(spec, 32.0);
	
	gl_FragColor	= vec4(min(kfinal + spec, 1.0), 1.0);
}

