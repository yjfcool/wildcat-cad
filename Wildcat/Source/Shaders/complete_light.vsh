void main(void)  {
	vec3 lightDir = vec3(0.0, 0.0, 1.0);
	vec3 norm = gl_NormalMatrix * gl_Normal;
	float intensity = dot(norm, lightDir);

	//Compute the clip-space position
	gl_Position = ftransform();
	gl_FrontColor = vec4(intensity, intensity, intensity, 1.0);
}