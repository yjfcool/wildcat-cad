uniform sampler2DRect alphaTexture;
uniform vec2 texSize;


void main(void) {
	vec2 inPos = vec2((1.0 - gl_TexCoord[0].s) * texSize.x, (1.0 - gl_TexCoord[0].t) * texSize.y); 
	vec4 texColor = texture2DRect(alphaTexture, inPos);
	if (texColor.x == 0.0) discard;
//	gl_FragColor = gl_Color;	
	gl_FragColor = gl_Color * texColor.x;
//	gl_FragColor = vec4(gl_TexCoord[0].st, 0.0, 1.0);
//	gl_FragColor = texColor;
}