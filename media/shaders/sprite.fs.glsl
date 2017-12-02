#version 330

in vec2 vs_uv;
out vec4 color;

uniform sampler2D sprite_texture;
// uniform sampler2D sprite_normal_texture;

void main() {

    color = texture(sprite_texture, vs_uv);
}
