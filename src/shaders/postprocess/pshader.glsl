uniform sampler2D Texture0;
uniform sampler2D Texture1;
uniform vec2 bgl_TextureCoordinateOffset[9];

uniform float screen_width;
uniform float screen_height;

const float edgeForce = 0.6; // The force of the outline blackness
const float depthThresh = 0.2; // The initial(near value) edge threshold value for inking

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

	 mat3 xMat = mat3(
		-xstep, 0, xstep,
		-xstep, 0, xstep,
		-xstep, 0, xstep );

	 mat3 yMat = mat3(
		-ystep, -ystep, -ystep,
		0, 		0, 		0,
		+ystep, +ystep, +ystep); 

	float ratio = screen_width / screen_height;

	vec4 color = texture2D( Texture0 , vec2( gl_TexCoord[0] ) );
	float depth = texture2D( Texture1 , vec2( gl_TexCoord[0] ) ).r;
	
	//Permet de scaler la profondeur
	depth = LinearizeDepth(depth);
	
	gl_FragColor = color;
	
	float sample[9];
	vec4 texcol = texture2D(Texture0, gl_TexCoord[0].st);
	
	float colDifForce = 0.0;
	
	for(int x = 0; x < 3; ++x){
		for(int y = 0; y < 3; ++y){
			sample[x+y] = LinearizeDepth( texture2D(Texture1, gl_TexCoord[0].st + vec2(xMat[x][y], yMat[x][y])).r);
			colDifForce += depth - sample[x+y];
		}
	}
	
	if(abs(colDifForce) > ( sample[4] * depthThresh ) * 0.1){
		gl_FragColor = vec4(vec3(texcol*edgeForce), 1.0);
	}	
	else{
		gl_FragColor = vec4(texcol);
	}
}