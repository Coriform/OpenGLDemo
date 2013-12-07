#version 330 core

// CLEAN THIS SHADER UP!


layout(location = 0) out vec4 outColor;

in vec2 Texcoord;
uniform sampler2D tBlur;
uniform sampler2D tDepth;


uniform float blurAmount;     // The sigma value for the gaussian function: higher value means more blur
                         // A good value for 9x9 is around 3 to 5
                         // A good value for 7x7 is around 2.5 to 4
                         // A good value for 5x5 is around 2 to 3.5
                         // ... play around with this based on what you need :)

uniform float blurPixels;

uniform vec2 blurSize;  // This should usually be equal to
                         // 1.0f / texture_pixel_width for a horizontal blur, and
                         // 1.0f / texture_pixel_height for a vertical blur.


const float pi = 3.14159265f;

const vec2 blurMultiplyVec = vec2(1.0,1.0);

void main() 
{

  float depth = texture2D(tDepth, Texcoord).z;

  float sigma = blurAmount * depth;

  // Incremental Gaussian Coefficent Calculation (See GPU Gems 3 pp. 877 - 889)
  vec3 incrementalGaussian;
  incrementalGaussian.x = 1.0f / (sqrt(2.0f * pi) * sigma);
  incrementalGaussian.y = exp(-0.5f / (sigma * sigma));
  incrementalGaussian.z = incrementalGaussian.y * incrementalGaussian.y;

  vec4 avgValue = vec4(0.0f, 0.0f, 0.0f, 0.0f);
  float coefficientSum = 0.0f;

  // Take the central sample first...
  avgValue += texture2D(tBlur, Texcoord) * incrementalGaussian.x;
  coefficientSum += incrementalGaussian.x;
  incrementalGaussian.xy *= incrementalGaussian.yz; 

  // Go through the remaining 8 vertical samples (4 on each side of the center)
  for (float i = 1.0f; i <= blurPixels; i++) 
  { 
    avgValue += texture2D(tBlur, Texcoord - i * blurSize) * incrementalGaussian.x;         
    avgValue += texture2D(tBlur, Texcoord + i * blurSize) * incrementalGaussian.x;         
    coefficientSum += 2 * incrementalGaussian.x;
    incrementalGaussian.xy *= incrementalGaussian.yz;
  }

  outColor = avgValue / coefficientSum;
}