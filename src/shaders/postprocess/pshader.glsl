uniform sampler2D Texture0;
uniform sampler2D Texture1;
uniform vec2 bgl_TextureCoordinateOffset[9];

uniform float screen_width;
uniform float screen_height;

const float edgeForce = 0.6; // The force of the outline blackness
const float depthThresh = 0.001; // The initial(near value) edge threshold value for inking

float LinearizeDepth(float z)
{
	float n = 0.5; // camera z near
  	float f = 10000.0; // camera z far
  	return (2.0 * n) / (f + n - z * (f - n));
}

void main (void)
{
	float xstep = 1.0/screen_width;
	float ystep = 1.0/screen_height;
	float ratio = screen_width / screen_height;

	vec4 color = texture2D( Texture0 , vec2( gl_TexCoord[0] ) );
	float depth = texture2D( Texture1 , vec2( gl_TexCoord[0] ) ).r;
	
	//Permet de scaler la profondeur
	depth = LinearizeDepth(depth);
	
	float sample[9];
	vec4 texcol = texture2D(Texture0, gl_TexCoord[0].st);
	
	float colDifForce = 0.0;
	
	for(int i = 0; i < 9; ++i){
		sample[i] = LinearizeDepth( texture2D(Texture1, gl_TexCoord[0].st + bgl_TextureCoordinateOffset[i]).r);
		colDifForce += depth - sample[i];
	}
	
	if(abs(colDifForce) > ( sample[4] * depthThresh ) / 0.5){
		gl_FragColor = vec4(vec3(texcol*edgeForce), 1.0);
	}	
	else{
		gl_FragColor = vec4(texcol);	
	}
}