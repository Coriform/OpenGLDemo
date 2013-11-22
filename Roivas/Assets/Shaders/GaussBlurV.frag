#version 330 core


layout(location = 0) out vec4 outColor;

uniform sampler2D sTexture;

in vec2 Texcoord;

float vGaussianBlur[10] ;

//= { 0.0882357, 0.0957407, 0.101786, 0.106026, 0.108212, 0.108212, 0.106026, 0.101786, 0.0957407, 0.0882357 };

float log_conv ( float x0, float X, float y0, float Y )
{
	return ( X + log( x0 + (y0 * exp(Y - X) ) ) );
}

vec4 LogGaussianFilter()
{

	vGaussianBlur[0] = 0.0882357;
	vGaussianBlur[1] = 0.0957407;
	vGaussianBlur[2] = 0.101786;
	vGaussianBlur[3] = 0.106026;
	vGaussianBlur[4] = 0.108212;
	vGaussianBlur[5] = 0.108212;
	vGaussianBlur[6] = 0.106026;
	vGaussianBlur[7] = 0.101786;
	vGaussianBlur[8] = 0.0957407;
	vGaussianBlur[9] = 0.0882357;

	float vSample[ 10 ];

	for (int i = 0; i < 10; i++)
	{
		float fOffSet = i - 4.5;
		vec2 vTexCoord = vec2( Texcoord.x + fOffSet, Texcoord.y + fOffSet );
		vSample[i] = texture2D( sTexture, vTexCoord ).r;
	}

	float fAccum;
	fAccum = log_conv( vGaussianBlur[0], vSample[0], vGaussianBlur[1], vSample[1] );

	for (int i = 2; i < 10; i++)
	{
		fAccum = log_conv( 1.0, fAccum, vGaussianBlur[i], vSample[i] );
	}

	return vec4( fAccum, 0.0, 0.0, 0.0);
}

void main()
{
	outColor = vec4(1,1,1,1);//LogGaussianFilter();
}








uniform float sigma;     // 2-3.5

uniform float blurSize;  // This should usually be equal to
// 1.0 / texture_pixel_width for a horizontal blur, and
// 1.0 / texture_pixel_height for a vertical blur.

uniform sampler2D blurSampler;  // Texture that will be blurred by this shader

const float pi = 3.14159265;



////
const float numBlurPixelsPerSide = 2.0;
const vec2  blurMultiplyVec      = vec2(0.0, 1.0);
////


void main() {
    
    // Incremental Gaussian Coefficent Calculation (See GPU Gems 3 pp. 877 - 889)
    vec3 incrementalGaussian;
    incrementalGaussian.x = 1.0 / (sqrt(2.0 * pi) * sigma);
    incrementalGaussian.y = exp(-0.5 / (sigma * sigma));
    incrementalGaussian.z = incrementalGaussian.y * incrementalGaussian.y;
    
    vec4 avgValue = vec4(0.0, 0.0, 0.0, 0.0);
    float coefficientSum = 0.0;
    
    // Take the central sample first...
    avgValue += texture2D(blurSampler, gl_TexCoord[0].xy) * incrementalGaussian.x;
    coefficientSum += incrementalGaussian.x;
    incrementalGaussian.xy *= incrementalGaussian.yz;
    
    // Go through the remaining 8 vertical samples (4 on each side of the center)
    for (float i = 1.0; i <= numBlurPixelsPerSide; i++) { 
        avgValue += texture2D(blurSampler, gl_TexCoord[0].xy - i * blurSize * 
        blurMultiplyVec) * incrementalGaussian.x;         
        avgValue += texture2D(blurSampler, gl_TexCoord[0].xy + i * blurSize * 
        blurMultiplyVec) * incrementalGaussian.x;         
        coefficientSum += 2.0 * incrementalGaussian.x;
        incrementalGaussian.xy *= incrementalGaussian.yz;
    }
    
    gl_FragColor = avgValue / coefficientSum;
}