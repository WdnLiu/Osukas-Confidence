

#version 330 core

in vec2 v_uv;
out vec4 FragColor;

uniform vec3 u_color;
uniform float u_percentage;
uniform float u_decrease;

void main() {


    vec3 finalColor = u_color;

    // Set the color for the remaining fragments
    FragColor = vec4(finalColor* v_uv.y, 1.0);
}