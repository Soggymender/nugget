#version 330 core

#define DELTA 0.00001
#define TAU 6.28318530718
#define NOISE_TEXTURE_SIZE 256.0
#define NOISE_TEXTURE_PIXEL_COUNT (NOISE_TEXTURE_SIZE * NOISE_TEXTURE_SIZE)

out vec4 FragColor;

uniform sampler2D texture1; // Main render sampler.
uniform sampler2D texture2; // Noise sampler.

in vec4 v_vColour;
in vec2 v_vTexcoord;

// MAIN CONTROLLER UNIFORMS
uniform float intensity;       // overall effect intensity, 0-1 (no upper limit)
uniform float time;            // global timer variable
uniform vec2  resolution;      // screen resolution
uniform float rngSeed;         // seed offset (changes configuration around)
uniform vec3  randomValues;    // random values changing every frame, passed in by the CPU

//TUNING
uniform float lineSpeed;       // line speed
uniform float lineDrift;       // horizontal line drifting
uniform float lineResolution;  // line resolution
uniform float lineVertShift;   // wave phase offset of horizontal lines
uniform float lineShift;       // horizontal shift
uniform float jumbleness;      // amount of "block" glitchiness
uniform float jumbleResolution;// resolution of blocks
uniform float jumbleShift;     // texture shift by blocks  
uniform float jumbleSpeed;     // speed of block variation
uniform float dispersion;      // color channel horizontal dispersion
uniform float channelShift;    // horizontal RGB shift
uniform float noiseLevel;      // level of noise
uniform float shakiness;       // horizontal shakiness
//

vec4 extractRed(vec4 col){
    return vec4(col.r, 0., 0., col.a);
}

vec4 extractGreen(vec4 col){
    return vec4(0., col.g, 0., col.a);
}

vec4 extractBlue(vec4 col){
    return vec4(0., 0., col.b, col.a);
}

// Replacement for the mirror address mode, hopefully nobody needs filtering.
vec2 mymirror(vec2 v) {
    return abs((fract((v * 0.5) + 0.5) * 2.0) - 1.0);
}

vec2 downsample(vec2 v, vec2 res) {    
    // Division by zero protected by uniform getters.
    return floor(v * res) / res;
}

// Fetches four random values from an RGBA noise texture
vec4 whiteNoise(vec2 coord, vec2 texelOffset) {
    vec2 offset = downsample(vec2(rngSeed * NOISE_TEXTURE_SIZE, rngSeed) + texelOffset, vec2(NOISE_TEXTURE_SIZE));
    vec2 ratio = resolution / vec2(NOISE_TEXTURE_SIZE);
    return texture(texture2, (coord * ratio) + offset); 
}

// Fetch noise texture texel based on single offset in the [0-1] range
vec4 random(float dataOffset) {
    vec2 halfTexelSize = vec2((0.5 / NOISE_TEXTURE_SIZE));
    float offset = rngSeed + dataOffset;    
    return texture(texture2, vec2(offset * NOISE_TEXTURE_SIZE, offset) + halfTexelSize); 
}

// Jumble coord generation
vec2 jumble(vec2 coord){
    // Static branch.
    if ((jumbleShift * jumbleness * jumbleResolution) < DELTA) {
        return vec2(0.0);
    }
        
    vec2 gridCoords = (coord * jumbleResolution) / (NOISE_TEXTURE_SIZE * 0.0245);
    float jumbleTime = mod(floor(time * 0.02 * jumbleSpeed), NOISE_TEXTURE_PIXEL_COUNT);
    vec2 offset = random(jumbleTime / NOISE_TEXTURE_PIXEL_COUNT).ga * jumbleResolution;
    vec4 cellRandomValues = whiteNoise(gridCoords, vec2(jumbleResolution * -10.0) + offset);
    return (cellRandomValues.ra - 0.5) * jumbleShift * floor(min(0.99999, cellRandomValues.b) + jumbleness);
}

// Horizontal line offset generation
float lineOffset(vec2 coord) {
    // Static branch.
    if (lineShift < DELTA) {
        return 0.0;
    }
    
    // Wave offsets
    vec2 waveHeights = vec2(50.0 * lineResolution, 25.0 * lineResolution);    
    vec4 lineRandom = whiteNoise(downsample(v_vTexcoord.yy, waveHeights), vec2(0.0));
    float driftTime = v_vTexcoord.y * resolution.y * 2.778;
    
    // XY: big waves, ZW: drift waves
    vec4 waveTimes = (vec4(downsample(lineRandom.ra * TAU, waveHeights) * 80.0, driftTime + 2.0, (driftTime * 0.1) + 1.0) + (time * lineSpeed)) + (lineVertShift * TAU);
    vec4 waveLineOffsets = vec4(sin(waveTimes.x), cos(waveTimes.y), sin(waveTimes.z), cos(waveTimes.w));
    waveLineOffsets.xy *= ((whiteNoise(waveTimes.xy, vec2(0.0)).gb - 0.5) * shakiness) + 1.0;
    waveLineOffsets.zw *= lineDrift;
    return dot(waveLineOffsets, vec4(1.0));
}

void main()
{
    // Sample random high-frequency noise
    vec4 randomHiFreq = whiteNoise(v_vTexcoord, randomValues.xy);
    
    // Apply line offsets
    vec2 offsetCoords = v_vTexcoord;
    offsetCoords.x += ((((2.0 * randomValues.z) - 1.0) * shakiness * lineSpeed) + lineOffset(offsetCoords)) * lineShift * intensity;
    
    // Apply jumbles
    offsetCoords += jumble(offsetCoords) * intensity * intensity * 0.25;
        
    // Channel split
    vec2 shiftFactors = (channelShift + (randomHiFreq.rg * dispersion)) * intensity;
    vec4 outColour;
    
    // Static branch.
//    if (((channelShift + dispersion) * intensity) < DELTA) {
        outColour = texture(texture1, mymirror(offsetCoords));
  //  } else {
  //      outColour = extractRed(texture(texture1, mymirror(offsetCoords + vec2(shiftFactors.r, 0.0)))) + extractBlue(texture(texture1, mymirror(offsetCoords + vec2(-shiftFactors.g, 0.0)))) + extractGreen(texture(texture1, mymirror(offsetCoords)));
    //}
    
    // Add noise    
    outColour.rgb *= (vec3(.55, .5, .4) * randomHiFreq.gab * intensity * noiseLevel) + 1.0;
        
    FragColor = v_vColour * outColour;
    //FragColor = outColour;

    //FragColor = outColour * randomHiFreq;


    // outColour = input texture or pixel color.
    // v_vColour looks white or very light grey.
}