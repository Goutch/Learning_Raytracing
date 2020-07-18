#version 330 core
out vec4 FragColor;
in vec2 uv;
uniform sampler2D texture_0;
const float exposure=1.;
const float gamma=1.;

void main()
{
    vec4 color=texture(texture_0, uv);
    FragColor = vec4(pow(vec3(1.0) - exp(-color.rgb * exposure), vec3(1.0 / gamma)), color.a);
}