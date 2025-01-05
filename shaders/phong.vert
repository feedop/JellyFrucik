#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;

out vec3 FragPos;                 // Fragment position for the fragment shader
out vec3 FragNormal;              // Normal vector for lighting

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 frag = model * vec4(aPos, 1.0);
    FragPos = vec3(frag);
    FragNormal = normalize(vec3(model * vec4(aNorm, 0.0)));
    gl_Position = projection * view * frag;
}