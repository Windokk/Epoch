#version 430 core

struct Light {
    int type;
    float intensity;
    vec3 position;
    vec3 direction;
    float radius;
    vec3 color;
    float innerCutoff;
    float outerCutoff;
    bool castShadow;
};

layout(std430, binding = 0) buffer LightBuffer {
    Light lights[];
};

out vec4 fragColor;

in vec2 texCoord;
in vec4 color;
in vec3 worldPos;
in vec3 worldNormal;

uniform sampler2D albedo;
uniform int lightNB;

uniform vec3 camPos;

// Shadow maps
uniform sampler2D shadow_dirShadowMaps[16];
uniform mat4 shadow_dirLightSpaceMatrices[16];

uniform sampler2D shadow_spotShadowMaps[16];
uniform mat4 shadow_spotLightSpaceMatrices[16];

uniform samplerCubeArray shadow_pointShadowMapArray;
uniform float shadow_pointLightFarPlanes[16];

// PBR values
const float PI = 3.141592653589793;
uniform float metallic;
uniform float roughness;

vec3 gridSamplingDisk[20] = vec3[](
    vec3(1,1,1), vec3(1,-1,1), vec3(-1,-1,1), vec3(-1,1,1), 
    vec3(1,1,-1), vec3(1,-1,-1), vec3(-1,-1,-1), vec3(-1,1,-1),
    vec3(1,1,0), vec3(1,-1,0), vec3(-1,-1,0), vec3(-1,1,0),
    vec3(1,0,1), vec3(-1,0,1), vec3(1,0,-1), vec3(-1,0,-1),
    vec3(0,1,1), vec3(0,-1,1), vec3(0,-1,-1), vec3(0,1,-1)
);

// -------------------- Shadow Functions --------------------

float ShadowCalculation(sampler2D shadowMap, vec3 lightDir, mat4 lightSpaceMatrix) {
    vec4 fragPosLightSpace = lightSpaceMatrix * vec4(worldPos, 1.0);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0 || projCoords.z < 0.0)
        return 0.0;

    float currentDepth = projCoords.z;
    float bias = max(0.005 * (1.0 - dot(normalize(worldNormal), -lightDir)), 0.002);
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    for (int x = -1; x <= 1; ++x)
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }

    return shadow / 9.0;
}

float ShadowPointArray(int index, vec3 lightPos, vec3 fragPos, float farPlane) {
    vec3 fragToLight = fragPos - lightPos;
    float currentDepth = length(fragToLight);
    float shadow = 0.0;
    float bias = 0.05;
    float viewDistance = 10.0;
    float diskRadius = (1.0 + (viewDistance / farPlane)) / 25.0;

    for (int i = 0; i < 20; ++i) {
        vec3 sampleOffset = fragToLight + gridSamplingDisk[i] * diskRadius;
        float closestDepth = texture(shadow_pointShadowMapArray, vec4(sampleOffset, float(index))).r * farPlane;
        shadow += (currentDepth - bias > closestDepth) ? 1.0 : 0.0;
    }

    return shadow / 20.0;
}

// -------------------- Lighting --------------------

vec3 computeLightDisney(Light light, vec3 L, vec3 V, vec3 N, vec3 baseColor, float roughness, float metallic, float shadow, float attenuation) {
    vec3 Nn = normalize(N);
    vec3 Ln = normalize(L);
    vec3 Vn = normalize(V);
    vec3 H = normalize(Vn + Ln);

    float NdotL = max(dot(Nn, Ln), 0.0);
    float NdotV = max(dot(Nn, Vn), 0.0);
    float NdotH = max(dot(Nn, H), 0.0);
    float VdotH = max(dot(Vn, H), 0.0);

    vec3 F0 = mix(vec3(0.04), baseColor, metallic);

    // GGX Distribution
    float a = roughness * roughness;
    float a2 = a * a;
    float denom = (NdotH * NdotH) * (a2 - 1.0) + 1.0;
    float D = a2 / (PI * denom * denom);

    // Geometry
    float k = (roughness + 1.0);
    k = (k * k) / 8.0;
    float G_V = NdotV / (NdotV * (1.0 - k) + k);
    float G_L = NdotL / (NdotL * (1.0 - k) + k);
    float G = G_V * G_L;

    // Fresnel
    vec3 F = F0 + (1.0 - F0) * pow(1.0 - VdotH, 5.0);

    // Specular BRDF
    vec3 numerator = D * G * F;
    float denominator = 4.0 * NdotV * NdotL + 0.001;
    vec3 specular = numerator / denominator;

    // Diffuse term (energy conserving)
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;
    vec3 diffuse = kD * baseColor / 3.14159265359;

    // Combine with light color, intensity, NdotL, shadow, attenuation
    vec3 Lo = (diffuse + specular) * NdotL * light.color * light.intensity * (1.0 - shadow) * attenuation;

    return Lo;
}

void main() {
    vec3 baseColor = texture(albedo, texCoord).rgb;

    vec3 N = normalize(worldNormal);
    vec3 V = normalize(camPos - worldPos);
    vec3 result = vec3(0.0);

    int dirIdx = 0, pointIdx = 0, spotIdx = 0;

    for (int i = 0; i < lightNB; ++i) {
        Light l = lights[i];

        float shadow = 1.0;
        float attenuation = 1.0;
        vec3 L = vec3(0.0);

        if (l.type == 0) { // Directional
            L = normalize(-l.direction);
            attenuation = 1.0;  // no attenuation for directional
            if (l.castShadow){
                shadow = ShadowCalculation(shadow_dirShadowMaps[dirIdx], l.direction, shadow_dirLightSpaceMatrices[dirIdx]);
                dirIdx++;
            }
        }
        else if (l.type == 1) { // Point
            vec3 toLight = l.position - worldPos;
            float dist = length(toLight);
            L = normalize(toLight);

            attenuation = clamp(1.0 - (dist / l.radius), 0.0, 1.0);
            attenuation *= attenuation;

            if (l.castShadow){
                shadow = ShadowPointArray(pointIdx, l.position, worldPos, shadow_pointLightFarPlanes[pointIdx]);
                pointIdx++;
            }
        }
        else if (l.type == 2) { // Spot
            
            vec3 toFrag = worldPos - l.position;


            float dist = length(toFrag);
            if (dist > l.radius) 
                continue;

            L = normalize(l.position - worldPos);

            float angleFactor = dot(normalize(l.direction), normalize(toFrag));

            float spotEffect = clamp((angleFactor - l.outerCutoff) / (l.innerCutoff - l.outerCutoff), 0.0, 1.0);

            attenuation = clamp(1.0 - (dist / l.radius), 0.0, 1.0);
            attenuation *= attenuation;
            attenuation *= spotEffect;

            if (l.castShadow) {
                shadow = ShadowCalculation(shadow_spotShadowMaps[spotIdx], l.direction, shadow_spotLightSpaceMatrices[spotIdx]);
                spotIdx++;
            }

        }

        result += computeLightDisney(l, L, V, N, baseColor, roughness, metallic, shadow, attenuation);
    }

    vec3 color = baseColor * 0.1 + result;

    fragColor = vec4(color, 1.0);
}