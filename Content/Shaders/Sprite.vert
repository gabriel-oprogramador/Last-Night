#version 330 core
layout(location = 0) in vec3 aVertPos;
layout(location = 1) in vec2 aTexCoord;

uniform mat4 uProj;
uniform mat4 uModel;

out vec2 vTexCoord;

void main() {
  gl_Position = uProj * uModel * vec4(aVertPos, 1);
  vTexCoord = aTexCoord;
}
