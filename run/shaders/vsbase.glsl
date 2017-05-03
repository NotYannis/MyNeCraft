varying vec3 normal;
varying vec3 vertex_to_light_vector;
varying vec4 color;

const float PI = 3.141592653589793238462643383;

uniform float elapsed;
uniform mat4 view;
uniform mat4 invertView;

vec3 computeNormal( vec3 pos, 
                    vec3 tangent, 
                    vec3 binormal, 
                    float phase)
{
	mat3 J;
	
	float dist = sqrt(pos.x*pos.x + pos.z*pos.z);
	float jac_coef = cos(2 * PI * dist * phase) / (dist+0.00001);
	
	// A matrix is an array of column vectors so J[2] is 
	// the third column of J.
	
	J[0][0] = 1.0;
	J[0][1] = 0.0;
	J[0][2] = jac_coef * pos.x;
	
	J[1][0] = 0.0;
	J[1][1] = 1.0;
	J[1][2] = 0.0;

	J[2][0] = 1.0;
	J[2][1] = 0.0;
	J[2][2] = jac_coef * pos.z;
	
	vec3 u = J * tangent;
	vec3 v = J * binormal;
	
	vec3 n = cross(v, u);
	return normalize(n);
}

void main()
{
	// Transforming The Vertex
	gl_Position = gl_ModelViewMatrix * gl_Vertex;
	gl_Position = invertView * gl_Position;

	//Couleur
	color = gl_Color;

	float dist = sqrt(gl_Position.x * gl_Position.x + gl_Position.y * gl_Position.y);
	float phase = elapsed * 0.0004;

	normal = gl_NormalMatrix * gl_Normal;

	if(color.b > 0.7){
		gl_Position.z += sin(dist * 2 * PI * phase) * 3;
		gl_Position.y += cos(dist * 2 * PI * phase) * 2;

		vec3 tangent; 
		vec3 binormal; 
	
		vec3 c1 = cross(gl_Normal, vec3(0.0, 0.0, 1.0)); 
		vec3 c2 = cross(gl_Normal, vec3(0.0, 1.0, 0.0)); 
	
		if(length(c1)>length(c2))
		{
			tangent = c1;	
		}
		else
		{
			tangent = c2;	
		}
	
		tangent = normalize(tangent);
	
		binormal = cross(gl_Normal, tangent); 
		binormal = normalize(binormal);

		normal = computeNormal( gl_Position.xyz, 
	                                 tangent.xyz, 
	                                 binormal, 
	                                 phase);

		normal = normalize(gl_NormalMatrix * normal);
	}



	gl_Position = view * gl_Position;
	gl_Position = gl_ProjectionMatrix * gl_Position;

	// Transforming The Normal To ModelView-Space

	//Direction lumiere
	vertex_to_light_vector = vec3(gl_LightSource[0].position);
}