#version 330 core
layout(location=0) in vec3 in_position;
layout(location=1) in vec3 in_color;
layout(location=2) in vec2 in_texCoord;

out vec3 ex_color;
out vec2 ex_texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
    gl_Position = projection * view * model * vec4(in_position, 1);
    ex_color = in_color;
	ex_texCoord = vec2(in_texCoord.x,in_texCoord.y);
}