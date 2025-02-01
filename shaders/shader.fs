#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

uniform float u_intensity;  // Input parameter to control the static
uniform sampler2D screenTexture;  // The input texture, e.g., the screen or background texture

uniform float time;  // The time passed since the start of the program

float random(vec2 uv)
{
    return fract(sin(dot(uv, vec2(12.9898, 78.233))) * 43758.5453);
}

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
}
//void main()
//{
//    FragColor = texture(ourTexture, TexCoord);
//}