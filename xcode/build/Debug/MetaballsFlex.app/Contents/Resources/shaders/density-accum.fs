#version 410

in GSOUTPUT
{
  vec2 uv;
  vec3 viewPos;
  float radius;
  vec4 color;
  vec3 sphereCenter;
} INPUT;

uniform float Scale;
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform int screenWidth;
uniform int screenHeight;

uniform sampler2D lambdaTexture;

layout(location = 0) out vec4 out_density;

// r = distance from center for current sample
// R = distance of the sphere forming the metaball
float density(float r, float R)
{
  return 16.0 / 9.0 * (1.0 - (r / (2.0 * R))*(r / (2.0 * R))) * (1.0 - (r / (2.0 * R))*(r / (2.0 * R)));
}

void main(void)
{
  float lensqr = dot(INPUT.uv, INPUT.uv);
  if (lensqr > 1.0) discard;

  vec2 texCoords = gl_FragCoord.xy / vec2(screenWidth, screenHeight);
  vec4 fragViewPos = texture(lambdaTexture, texCoords);
  float densityVal = density(distance(fragViewPos.xyz, INPUT.sphereCenter), 1.0);

  //out_density = fragViewPos;
  out_density = vec4(densityVal, densityVal, densityVal, 1.0);
  //out_density = vec4(fragViewPos.z, fragViewPos.z, fragViewPos.z, 1.0);
  //out_density = vec4(1, 0, 0, 1);

}
