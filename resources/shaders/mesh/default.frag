#version 430 core

out vec4 fragColor;

in vec2 texCoord;
in vec4 color;
in vec3 worldPos;
in vec3 worldNormal;

uniform int useTexture;
uniform sampler2D texture1;
uniform int lightNB;

struct Light {
    int type;
    float intensity;
    vec3 position;
    vec3 direction;
    float radius;
    vec3 color;
    float innerCutoff;
    float outerCutoff;
    float _padding;
};

layout(std430, binding = 0) buffer LightBuffer {
    Light lights[];
};

vec3 computeDirectionalLight(Light light) {
    return light.color * light.intensity;
}


vec3 computePointLight(Light light, vec3 fragPos, vec3 normal) {
    vec3 toLight = light.position - fragPos;
    float dist = length(toLight);
    if (dist > light.radius) return vec3(0.0);

    vec3 lightDir = normalize(toLight);
    float diff = max(dot(normal, lightDir), 0.0);

    float attenuation = 1.0 - (dist / light.radius);
    attenuation *= attenuation;

    return light.color * light.intensity * diff * attenuation;
}

vec3 computeSpotLight(Light light, vec3 fragPos, vec3 normal) {
    vec3 toFrag = fragPos - light.position;
    float dist = length(toFrag);
    if (dist > light.radius) return vec3(0.0);

    vec3 lightDir = normalize(-light.direction);
    vec3 fragDir = normalize(toFrag);

    float angleFactor = dot(lightDir, fragDir); 

    float spotEffect = smoothstep(light.outerCutoff, light.innerCutoff, angleFactor);
    if (spotEffect <= 0.0) return vec3(0.0);

    float diff = max(dot(normal, -fragDir), 0.0);

    float att = 1.0 - (dist / light.radius);
    att *= att;
    att *= spotEffect;

    return light.color * light.intensity * diff * att;
}

void main()
{
    if(useTexture == 1)
    {
        vec4 baseColor = texture(texture1, texCoord);
        vec3 normal = normalize(worldNormal);

        vec3 totalLight = vec3(0.0);

        for (int i = 0; i < lightNB; ++i) {
            Light l = lights[i];

            if (l.type == 0) // Directional
                totalLight += computeDirectionalLight(l);
            else if (l.type == 1) // Point
                totalLight += computePointLight(l, worldPos, normal);
            else if (l.type == 2) // Spot
                totalLight += computeSpotLight(l, worldPos, normal);
        }

        vec3 litColor = clamp(totalLight, 0.0, 1.0);
        fragColor = vec4(baseColor.rgb * litColor, baseColor.a);
    }
    else
    {
        fragColor = color;
    }
}
