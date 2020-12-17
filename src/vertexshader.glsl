#version 330 core

layout( location = 0 ) in vec3 vertexPos;
layout( location = 1 ) in vec3 vertexNormal;

// Texture coordinates
layout( location = 2 ) in vec2 vertexTexCoord;

uniform mat4 mvp;
uniform mat4 mv;
uniform mat4 mv_ti; // transpose inverse of MV

// Position and normal coordinates in View Space
out vec3 fragPos;
out vec3 fragNormal;

// Fragment texture coordinates
out vec2 fragUV;

void main()
{
    gl_Position = mvp * vec4( vertexPos, 1 );

    fragPos = (mv * vec4( vertexPos, 1 ) ).xyz;

    fragNormal = (mv_ti * vec4( vertexNormal, 1 ) ).xyz;

//    fragUV = vertexTexCoord;
}
