#version 410

in VSOUTPUT
{
  vec3 positionVS;
} INPUT[];

out GSOUTPUT
{
  vec2 uv;
  vec3 viewPos;
  float radius;
  vec4 color;
  vec3 sphereCenter;
} OUTPUT;

uniform mat4 projectionMatrix;
uniform float Scale;

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

void main()
{
  //if (INPUT[0].atomRadius <= 0) return;

  float radius = 1.0;

  vec3 billboardPosVS = INPUT[0].positionVS;
  vec3 viewDir = billboardPosVS - vec3(0);
  vec3 up = vec3(0.0, 1.0, 0.0);
  vec3 right = normalize(cross(viewDir, up)).xyz;

  OUTPUT.color = vec4(1.0, 1.0, 1.0, 1.0);
  OUTPUT.radius = 1.0;
  OUTPUT.sphereCenter = billboardPosVS;

  float triSide = 3.464;
  float triHalfSide = triSide / 2.0;
  float radiusScaled = radius * Scale;
  // 1
  vec3 offset = -right * 1.73205 * radiusScaled + -up * radiusScaled;
  OUTPUT.uv = vec2(-triHalfSide, -1.0);
  OUTPUT.viewPos = vec3(billboardPosVS + offset);
  gl_Position = projectionMatrix * vec4(billboardPosVS + offset, 1.0);
  EmitVertex();
  // 2
  offset = right * 1.73205 * radiusScaled + -up * radiusScaled;
  OUTPUT.uv = vec2(triHalfSide, -1.0);
  OUTPUT.viewPos = vec3(billboardPosVS + offset);
  gl_Position = projectionMatrix * vec4(billboardPosVS + offset, 1.0);
  EmitVertex();
  // 3
  offset = up * 2 * radiusScaled;
  OUTPUT.uv = vec2(0.0, 2.0);
  OUTPUT.viewPos = vec3(billboardPosVS + offset);
  gl_Position = projectionMatrix * vec4(billboardPosVS + offset, 1.0);
  EmitVertex();

  EndPrimitive();
}
