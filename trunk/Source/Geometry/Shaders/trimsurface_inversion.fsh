//Uniform Textures
uniform sampler2DRect				verts;
uniform sampler2DRect				surfData;
#define sd(i,j)						(texture2DRect(surfData, vec2(i,j)))

//Uniform Inputs
uniform ivec2						params;		// { surfWidth, surfHeight }


/*************************************************************************/


void main(void) {
	float dist, minDist = 100000.0;
	int u, v, markU, markV;
	vec4 surfPt;
	vec4 pt = texture2DRect(verts, floor(gl_FragCoord.xy));
	float paraU = 1.0 / (float(params.x) - 1.0);
	float paraV = 1.0 / (float(params.y) - 1.0);

	//Loop through all points on surface
	for (v=0; v<params.y; v++) {
		for (u=0; u<params.x; u++) {
			//Determine distance from point to surface point
			surfPt = sd(u,v);
			dist = distance(pt, surfPt);
			//Is this smaller than current smallest
			if (dist <= minDist) {
				//Capture the location
				markU = u;
				markV = v;
				minDist = dist;
			}
		}
	}

	//Now determine quadrant
	vec4 center = vec4( sd(markU, markV) );
	vec4 left, right, top, bottom;
	float leftDist, rightDist, bottomDist, topDist;
	//Set left point
	if (markU > 0) {
		left = vec4( sd(markU-1, markV) );
		leftDist = distance(left, pt);
	}
	else leftDist = 10000.0;
	//Set the right point
	if (markU < params.x-1) {
		right = vec4( sd(markU+1, markV) );
		rightDist = distance(right, pt);
	}
	else rightDist = 10000.0;
	//Set the bottom point
	if (markV > 0) {
		bottom = vec4( sd(markU, markV-1) );
		bottomDist = distance(bottom, pt);
	}
	else bottomDist = 10000.0;
	//Set the top point
	if (markV < params.y-1) {
		top = vec4( sd(markU, markV+1) );
		topDist = distance(top, pt);
	}
	else topDist = 10000.0;
	//Set the signs
	float hSign, vSign;
	vec4 hPt, vPt;
	if (leftDist < rightDist) { hSign = -1.0; hPt = left; }
	else { hSign = 1.0; hPt = right; }
	if (bottomDist < topDist) { vSign = -1.0; vPt = bottom; }
	else { vSign = 1.0; vPt = top; }

	//Convert from triangle values to [u,v]
	vec4 vDir = vPt - center;
	float vDirMag = length(vDir);
	vec4 hDir = hPt - center;
	float hDirMag = length(hDir);
	vec4 pDir = pt - center;
	float uDot = dot(hDir, pDir) / (hDirMag * hDirMag);
	float vDot = dot(vDir, pDir) / (vDirMag * vDirMag);
	float uValue = (paraU * float(markU)) + (uDot * paraU * hSign);
	float vValue = (paraV * float(markV)) + (vDot * paraV * vSign);
	//Bound u and v in [0,1]
	uValue = 1.0 - max(0.0, min(1.0, uValue));
	vValue = 1.0 - max(0.0, min(1.0, vValue));
	//Record [u,v] value into output texture
	gl_FragColor = vec4(uValue, vValue, 0.0, 1.0);
}


/*************************************************************************/

