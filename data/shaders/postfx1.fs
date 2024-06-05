varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;

void main()
{
    vec4 color = v_color;
    color.r = (color.r - 0.5) * contrast + 0.5;

	gl_FragColor = color;
}
