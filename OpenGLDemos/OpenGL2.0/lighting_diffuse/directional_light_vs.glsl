attribute vec4 v_coord;
attribute vec3 v_normal;
uniform mat4 m, v, p;
uniform mat3 m_3x3_inv_transp;
varying vec4 color;

// Directional light
struct lightSource
{
    vec4 direction;
	vec4 diffuse;
};

vec4 light_direction = vec4(-1.0, 1.0, -1.0, 0.0);
vec4 light_diffuse   = vec4(1.0, 1.0, 1.0, 1.0);
lightSource light0 = lightSource(light_direction, light_diffuse);

struct material
{
    vec4 diffuse;
};

material mymaterial = material(vec4(1.0, 0.8, 0.8, 1.0));

void main(void)
{
    mat4 mvp = p * v * m; // product of worl view and projection matrix
	vec3 normalDirection = normalize(m_3x3_inv_transp * v_normal);
	vec3 lightDirection  = normalize(vec3(light0.direction));

	vec3 I = vec3(light0.diffuse);
	vec3 K = vec3(mymaterial.diffuse);
	vec3 diffuseReflection = I * K * max(0.0, dot(normalDirection, lightDirection));

	color = vec4(diffuseReflection, 1.0);
	gl_Position = mvp * v_coord;
}