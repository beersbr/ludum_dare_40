#version 330 core

const vec4 verts[6] = vec4[6](vec4( -0.5,  0.5, 0.0, 1.0),
                              vec4( -0.5, -0.5, 0.0, 1.0),
                              vec4(  0.5,  0.5, 0.0, 1.0),
                              vec4(  0.5,  0.5, 0.0, 1.0),
                              vec4( -0.5, -0.5, 0.0, 1.0),
                              vec4(  0.5, -0.5, 0.0, 1.0));

const vec2 uvs[6] = vec2[6](vec2( 0.0, 1.0),
                            vec2( 0.0, 0.0),
                            vec2( 1.0, 1.0),
                            vec2( 1.0, 1.0),
                            vec2( 0.0, 0.0),
                            vec2( 1.0, 0.0));

out vec2 vs_uv;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main(void) {
    vs_uv = uvs[gl_VertexID];
    mat4 mvp = projection * view * model;
    gl_Position = mvp * verts[gl_VertexID];
}
