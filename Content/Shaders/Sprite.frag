#version 330 core

uniform vec4 uColor = vec4(1.0, 1.0, 1.0, 1.0);
uniform sampler2D uMainTex;
uniform vec4 uAnimCoord;

in vec2 vTexCoord;
out vec4 fragColor;

vec4 finalColor;

void main() {
  vec2 adjustCoords = mix(uAnimCoord.xy, uAnimCoord.zw, vTexCoord);
  finalColor = uColor * texture(uMainTex, adjustCoords);
  fragColor = finalColor;
}
