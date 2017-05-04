varying vec3 normal;
varying vec3 vertex_to_light_vector;
varying vec4 colorLevel;

uniform float ambientLevel;
uniform sampler2D color;
uniform int radius;
uniform int renderWidth;

float intensity( in vec4 color){
	return sqrt((color.x * color.x)+(color.y * color.y) + (color.z * color.z));
}

vec3 simple_edge_detection(in float step, in vec2 center){
	float center_intensity = intensity(texture2D(color, center));
	int darker_count = 0;
	float max_intensity = center_intensity;
	
	for(int i = -radius; i <= radius; i++){
		for(int j = -radius; j <= radius; j++){
			vec2 current_location = center + vec2(i*step, j*step);
			float current_intensity = intensity(texture2D(color, current_location));
			if(current_intensity < center_intensity){
				max_intensity = current_intensity;
			}
		}
	}
	
	if((max_intensity - center_intensity) > 0.01 * radius){
		if(darker_count / (radius * radius) < (1-(1/radius))){
			return vec3(0.0, 0.0, 0.0);
		}
	}
	
	return vec3(1.0, 1.0, 1.0);
}

void main()
{
	// Scaling The Input Vector To Length 1
	vec3 normalized_normal = normalize(normal);
	vec3 normalized_vertex_to_light_vector = normalize(vertex_to_light_vector);

	// Calculating The Diffuse Term And Clamping It To [0;1]
	float DiffuseTerm = clamp(dot(normal, vertex_to_light_vector), 0.0, 1.0);

	// Calculating The Final Color
	gl_FragColor = colorLevel * (DiffuseTerm*(1-ambientLevel) + ambientLevel);
	gl_FragColor.a = colorLevel.a;
	
	float step = 1.0/renderWidth;
	vec2 center_color = gl_TexCoord[0].st;
	gl_FragColor.xyz = simple_edge_detection(step, center_color);
	gl_FragColor.a = 0.0;
}