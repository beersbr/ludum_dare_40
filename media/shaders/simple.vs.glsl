#version 330 core

void main(void) {
    const vec4 verts[6] = vec4[6](vec4( -1.0,  1.0, 0.0, 1.0),
                                  vec4( -1.0, -1.0, 0.0, 1.0),
                                  vec4(  1.0,  1.0, 0.0, 1.0),
                                  vec4(  1.0,  1.0, 0.0, 1.0),
                                  vec4( -1.0, -1.0, 0.0, 1.0),
                                  vec4(  1.0, -1.0, 0.0, 1.0));

    gl_Position = verts[gl_VertexID];
}

