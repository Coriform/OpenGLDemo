#version 330 core

layout(location = 0) in vec3 position;

out vec2 Texcoord;

void main()
{
	gl_Position =  vec4(position, 1);
	Texcoord = (position.xy + vec2(1,1)) / 2.0;
}








uniform float sigma;     // 2-3.5

uniform float blurSize;  // This should usually be equal to
// 1.0 / texture_pixel_width for a horizontal blur, and
// 1.0 / texture_pixel_height for a vertical blur.

uniform sampler2D blurSampler;  // Texture that will be blurred by this shader

const float pi = 3.14159265;


////
const float numBlurPixelsPerSide = 2.0;
const vec2  blurMultiplyVec      = vec2(1.0, 0.0);
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