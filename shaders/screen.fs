#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;

uniform float pixelationIntensity;  // Controls the intensity of the pixelation (higher = more pixelated)
uniform float noiseAmount;

uniform float u_intensity;  // Input parameter to control the static

uniform float time;  // The time passed since the start of the program

float rand(vec2 co){
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

/*
void main()
{
    vec2 uv = TexCoords;

    // Determine the size of the blocks based on pixelation intensity
    float blockSize = pixelationIntensity; // Larger values = bigger blocks
  
    vec2 quantizedUv = floor(uv * blockSize) / blockSize;

    // Sample the texture at the quantized coordinates
    vec4 color = texture(screenTexture, quantizedUv);

    // Add some random noise to the color (this simulates a corrupted signal)
    float noise = rand(quantizedUv) * noiseAmount * 0.15f;

    // Apply the noise to the color (the higher the noiseAmount, the more noisy the effect)
    color.rgb += noise;

    FragColor = color;
}
*/

void main()
{ 
    FragColor = texture(screenTexture, TexCoords);
}