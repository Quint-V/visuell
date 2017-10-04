#version 430 core

layout(location=0) in vec3 position;
layout(location=1) in vec4 color;


uniform layout(location=10) mat4x4 Matrix;

out vec4 colorV;

// column major matrices;
// listings are flipped
// across diagonal, not
// like shown.

void main()
{
    gl_Position = Matrix * vec4(position, 1.0f);
	colorV = color;
}
