varying float lightIntensity;


void main(void)  {
	vec3 lightDir = vec3(0.0, 0.0, 1.0);
	vec3 norm = gl_NormalMatrix * gl_Normal;

	//Compute the clip-space position
	gl_Position = ftransform();
	gl_FrontColor = gl_Color;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	lightIntensity = dot(norm, lightDir);	
}