varying vec3 normal;
varying vec3 vertex_to_light_vector;
varying vec4 color;

const float PI = 3.141592653589793238462643383;

uniform float elapsed;
uniform mat4 view;
uniform mat4 invertView;

void createWave(){
	// Transforming The Vertex
	gl_Position = gl_ModelViewMatrix * gl_Vertex;
	gl_Position = invertView * gl_Position;

	//Couleur
	color = gl_Color;

	float dist = sqrt(gl_Position.x * gl_Position.x + gl_Position.y * gl_Position.y);
	float phase = elapsed * 0.4;

	normal = gl_NormalMatrix * gl_Normal;

	if(color.b > 0.7){
		gl_Position.z += sin(dist * 2 * PI + phase);
		gl_Position.y += cos(dist * 2 * PI + phase);
	}	

	// Transforming The Normal To ModelView-Space
	gl_Position = view * gl_Position;
	gl_Position = gl_ProjectionMatrix * gl_Position;	
}

void main()
{
	createWave();
	
	gl_TexCoord[1] = gl_MultiTexCoord0;
	gl_Position = ftransform();
	
	//Direction lumiere
	vertex_to_light_vector = vec3(gl_LightSource[0].position);
}