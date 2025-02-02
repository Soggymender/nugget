#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

uniform float pixelationIntensity;  // Controls the intensity of the pixelation (higher = more pixelated)
uniform float noiseAmount;

uniform float u_intensity;  // Input parameter to control the static
uniform sampler2D screenTexture;  // The input texture, e.g., the screen or background texture

uniform float time;  // The time passed since the start of the program

float rand(vec2 co){
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

/*
void main()
{
    vec2 uv = TexCoord;

    // Determine the size of the blocks based on pixelation intensity
    float blockSize = pixelationIntensity * 100.0; // Larger values = bigger blocks
 
 
    vec2 quantizedUv = floor(uv * blockSize) / blockSize;

    // Sample the texture at the quantized coordinates
    vec4 color = texture(ourTexture, quantizedUv);

    // Add some random noise to the color (this simulates a corrupted signal)
    float noise = rand(quantizedUv + vec2(uv.x, uv.y)) * noiseAmount;

    // Apply the noise to the color (the higher the noiseAmount, the more noisy the effect)
    color.rgb += noise;

    FragColor = color;
}
*/
/*
void main()
{
   // Create random noise based on UV coordinates
    float noise = random(TexCoord);
    
    // Scale the static intensity based on the time value (between 0 and 1)
    float staticIntensity = time;  // When time = 0, no static, when time = 1, full static
    
    // Apply the static noise, scaling it with the static intensity
    vec3 staticEffect = vec3(noise) * staticIntensity;
    
    // Optionally modulate the noise for a flicker effect (this can be adjusted)
    staticEffect *= 0.5 + 0.5 * sin(time);  // Flicker effect based on time
    
    // Combine static effect with the screen texture
    FragColor = texture(ourTexture, TexCoord);
    vec4 textureColor = FragColor;//texture(screenTexture, TexCoord);
    
    if (time == 1.0) {
        FragColor = vec4(staticEffect, 1.0); // Replace texture with full static
    } else {
        // Blend the static effect with the texture, scaling the static intensity
        FragColor = vec4(textureColor.rgb * (1.0 - staticIntensity) + staticEffect, textureColor.a);
    }
}*/

void main()
{
    vec2 newUV = floor(TexCoord * 640.0 / 10.0) / 640.0 * 10.0;


    FragColor = texture(ourTexture, newUV);
}