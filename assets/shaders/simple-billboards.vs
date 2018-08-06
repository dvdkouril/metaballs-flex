#version 410

layout(location = 0) in vec3 position;

out VSOUTPUT
{
  vec3 positionVS;
} OUTPUT;

uniform mat4 modelViewMatrix;

void main(void)
{
  vec3 pos = (modelViewMatrix * vec4(position, 1.0)).xyz;

  OUTPUT.positionVS = pos;
}
