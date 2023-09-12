#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D cubeTexture;

void main() {
    vec4 color = texture(cubeTexture, TexCoord);
    FragColor = vec4(color.r, color.g, color.b, color.a);
}
