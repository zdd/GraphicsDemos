// This fragment shader generate a red wireframe circle
/*
varying vec2 f_texcoord;
uniform sampler2D mytexture;

void main(void) {

	// Calculate pixel distance to circle center
	float d = distance(f_texcoord.xy, vec2(0.5, 0.5));

	// Discard fragments regarding the distance to the center.
	if (d > 0.5 || d < 0.45)
	    discard;

	gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
*/

// This fragment shader generate a colored circle
/*
varying vec2 f_texcoord;
uniform sampler2D mytexture;

void main(void) {

	vec2 pos = f_texcoord.xy - vec2(0.5, 0.5);

	// Discard fragments outside of the sphere
	if (length(pos) > 0.5)
	    discard;

	float r = 0.5;
	float x = pos.x;
	float y = pos.y;
	float z = sqrt(r * r - x * x - y * y);

	vec3 normal = normalize(vec3(x, y, z));

	gl_FragColor = vec4(normal, 1.0);
}
*/

varying vec2 f_texcoord;
uniform float timedelta;

void main(void) {

	vec2 pos = f_texcoord.xy - vec2(0.5, 0.5);

	// Discard fragments outside of the sphere
	if (length(pos) > 0.5)
	    discard;

	float r = 0.5;
	float x = pos.x;
	float y = pos.y;
	float z = sqrt(r * r - x * x - y * y);

	float lightX = cos(timedelta);
	float lightZ = sin(timedelta);
	vec3 lightDirection = vec3(lightX, 1.0, lightZ);

	// pixel normal, this is a simulate
	vec3 normal = normalize(vec3(x, y, z));

	// Light color
	vec3 I = vec3(1.0, 1.0, 1.0);

	// Material
	vec3 K = vec3(1.0, 1.0, 0.0);

	vec3 diffuseReflection = I * K * max(0.0, dot(lightDirection, normal));

	gl_FragColor = vec4(diffuseReflection, 1.0);
}

