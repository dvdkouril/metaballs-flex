#version 410
//#version 430
in vec3 position;
out vec2 texCoords;

void main(void)
{
    gl_Position = vec4(position, 1.0);
    texCoords = position.xy * vec2(0.5, 0.5) + vec2(0.5, 0.5);
}
