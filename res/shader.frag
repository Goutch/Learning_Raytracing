#version 330 core
out vec4 FragColor;
in vec2 uv;
uniform sampler2D color_texture;
uniform sampler2D normal_texture;
uniform vec2 resolution;
const float exposure=1.;
const float gamma=1.;

void main()
{
    vec4 center_color = texture2D(color_texture, uv);
    /*vec4 center_normal=texture2D(normal_texture, uv);
    vec4 color = vec4(0., 0., 0., 1.);
    if (length(center_normal)>0.01)
    {
        float total = 0.0;
        for (float x = -4.0; x <= 4.0; x += 1.0) {
            for (float y = -4.0; y <= 4.0; y += 1.0) {
                vec4 sample_color = texture2D(color_texture, uv + vec2(x, y) / resolution);
                vec4 sample_normal = texture2D(normal_texture, uv + vec2(x, y) / resolution);
                if (length(sample_normal)>0.01)
                {
                    float weight = 1.0 - abs(dot(sample_color.rgb - center_color.rgb, vec3(0.25)));
                    weight = pow(weight, 2.);
                    weight*=dot(sample_normal.xyz, center_normal.xyz);
                    if (weight>0.001)
                    {
                        color += sample_color * weight;
                        total += weight;
                    }
                }
            }
        }
        color.rgb/=total;
    }
    else
    {
        color=center_color;
    }*/

    FragColor = vec4(pow(vec3(1.0) - exp(-center_color.rgb * exposure), vec3(1.0 / gamma)), center_color.a);

}