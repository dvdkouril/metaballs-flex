#version 410
//#version 430
in vec2 texCoords;

uniform sampler2D normalTex;
//uniform sampler2D depthTex;
uniform sampler2D viewPosTex;
uniform sampler2D noiseTex;
uniform vec3 samples[64];
uniform mat4 projectionMatrix;

uniform float width;
uniform float height;

//~ SSAO settings
uniform float radius;
uniform float ssao_threshold; // = 0.5;
uniform float ssao_falloff; // = 0.1;

layout(location = 0) out vec4 ssao_output;

int kernelSize = 64;
//float radius = 0.0015;

//const vec2 noiseScale = vec2((1280.0-300) / 4.0, 720.0 / 4.0);

void main(void)
{
    //debug
    //ssao_output = vec4(1);
    //return;
    vec2 noiseScale = vec2(width / 4.0, height / 4.0);
    vec4 viewPos4 = texture(viewPosTex, texCoords).xyzw;
    if (viewPos4.w < 1.0) discard; // testing if this fragment has protein rendered on it, otherwise it's bg
    
    //vec3 viewPos = texture(viewPosTex, texCoords).xyz;
    vec3 viewPos = viewPos4.xyz;
    vec3 normal = texture(normalTex, texCoords).xyz;
    vec3 randomVec = texture(noiseTex, texCoords * noiseScale).xyz;
    
    normal = normalize(normal * 2.0 - 1.0);
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    
    float occlusion = 0.0;
    
    for(int i = 0; i < kernelSize; i++)
    {
        // if (length(viewPos) > 0.5) {
        //   break;
        // }
        vec3 posSample = TBN * samples[i];
        //vec3 posSample = samples[i];
        posSample = viewPos + posSample * radius;
        
        vec4 offset = vec4(posSample, 1.0);
        offset = projectionMatrix * offset;
        offset.xy /= offset.w;
        offset.xy = offset.xy * 0.5 + 0.5;
        
        vec4 sampleViewPos = texture(viewPosTex, offset.xy);
        float sampleDepth = sampleViewPos.z;
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(viewPos.z - sampleDepth));
        occlusion += (sampleDepth >= posSample.z ? 1.0 : 0.0) * rangeCheck;
    }
    
    // if (length(viewPos) > 0.5)
    //float ssao_threshold = 0.5;
    //float ssao_falloff = 0.1;
    //float occlusion_weight = smoothstep(ssao_threshold, ssao_threshold - ssao_falloff, length(viewPos));
    float occlusion_weight = smoothstep(ssao_threshold - ssao_falloff, ssao_threshold, length(viewPos));
    occlusion_weight = 1.0 - occlusion_weight;
    //float occlusion_weight = 0.5;
    occlusion_weight *= 0.8;
    occlusion = 1.0 - ((occlusion_weight * occlusion) / kernelSize);
    //occlusion = 0.0;
    ssao_output = vec4(occlusion, occlusion, occlusion, 1.0);
}
