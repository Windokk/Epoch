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

// Shadow maps
uniform sampler2D shadow_dirShadowMaps[16];
uniform sampler2D shadow_spotShadowMaps[16];
uniform samplerCubeArray shadow_pointShadowMapArray;

// Per-point light far planes (for depth comparison)
uniform float shadow_pointLightFarPlanes[16];

// Per-light space matrices for dir/spot lights
uniform mat4 shadow_lightSpaceMatrices[16];

vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float ShadowCalculation(sampler2D shadowMap, vec3 lightDir, mat4 lightSpaceMatrix)
{
    vec4 fragPosLightSpace = lightSpaceMatrix * vec4(worldPos, 1.0);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0 || projCoords.z < 0.0)
        return 0.0;

    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    vec3 normal = normalize(worldNormal);
    float bias = max(0.005 * (1.0 - dot(normal, normalize(-lightDir))), 0.002);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    return shadow;
}

float ShadowPointArray(int index, vec3 lightPos, vec3 fragPos, float farPlane)
{
    vec3 fragToLight = fragPos - lightPos;
    float currentDepth = length(fragToLight);

    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float viewDistance = 10;
    float diskRadius = (1.0 + (viewDistance / farPlane)) / 25.0;

    for (int i = 0; i < samples; ++i)
    {
        vec3 sampleOffset = fragToLight + gridSamplingDisk[i] * diskRadius;
        float closestDepth = texture(shadow_pointShadowMapArray, vec4(sampleOffset, float(index))).r;
        closestDepth *= farPlane; // Convert from [0,1] back to world distance

        if (currentDepth - bias > closestDepth)
            shadow += 1.0;
    }

    shadow /= float(samples);
    return shadow;
}

// Lighting
vec3 computeDirectionalLight(Light light, int index, vec3 fragPos, vec3 normal)
{
    vec3 lightDir = normalize(light.direction);
    float diff = max(dot(normal, -lightDir), 0.0);

    float shadow = 0.0;
    if (index >= 0)
        shadow = light.castShadow ? ShadowCalculation(shadow_dirShadowMaps[index], light.direction, shadow_lightSpaceMatrices[index]) : 0.0;
        
    return light.color * light.intensity * diff * (1.0 - shadow);
}

vec3 computePointLight(Light light, int index, vec3 fragPos, vec3 normal)
{
    vec3 toLight = light.position - fragPos;
    float dist = length(toLight);
    if (dist > light.radius) return vec3(0.0);

    vec3 lightDir = normalize(toLight);
    float diff = max(dot(normal, lightDir), 0.0);

    float att = 1.0 - (dist / light.radius);
    att *= att;

    float shadow = 0.0;
    if (index >= 0)
        shadow = light.castShadow ? ShadowPointArray(index, light.position, fragPos, shadow_pointLightFarPlanes[index]) : 0.0;

    return light.color * light.intensity * diff * att * (1.0 - shadow);
}

vec3 computeSpotLight(Light light, int index, vec3 fragPos, vec3 normal)
{
    vec3 toFrag = fragPos - light.position;
    float dist = length(toFrag);
    if (dist > light.radius) return vec3(0.0);

    vec3 fragDir = normalize(toFrag);
    vec3 lightDir = normalize(light.direction);

    float angleFactor = dot(lightDir, fragDir);

    if (angleFactor < light.outerCutoff) return vec3(0.0);

    float spotEffect = clamp((angleFactor - light.outerCutoff) / (light.innerCutoff - light.outerCutoff), 0.0, 1.0);

    float diff = max(dot(normal, -fragDir), 0.0);

    float att = 1.0 - (dist / light.radius);
    att *= att;
    att *= spotEffect;

    float shadow = 0.0;
    if (index >= 0)
        shadow = light.castShadow ? ShadowCalculation(shadow_spotShadowMaps[index], fragDir, shadow_lightSpaceMatrices[index]) : 0.0;

    return light.color * light.intensity * diff * att * (1.0 - shadow);
}

void main()
{
    vec4 baseColor = texture(albedo, texCoord);
    vec3 normal = normalize(worldNormal);
    vec3 totalLight = vec3(0.0);

    int dirShadowIndex = 0;
    int pointShadowIndex = 0;
    int spotShadowIndex = 0;

    for (int i = 0; i < lightNB; ++i) {
        Light l = lights[i];

        if (l.type == 0) { // Directional
            if (l.castShadow) {
                totalLight += computeDirectionalLight(l, dirShadowIndex, worldPos, normal);
                dirShadowIndex++;
            } else {
                totalLight += computeDirectionalLight(l, -1, worldPos, normal);
            }
        }
        else if (l.type == 1) { // Point
            if (l.castShadow) {
                totalLight += computePointLight(l, pointShadowIndex, worldPos, normal);
                pointShadowIndex++;
            } else {
                totalLight += computePointLight(l, -1, worldPos, normal);
            }
        }
        else if (l.type == 2) { // Spot
            if (l.castShadow) {
                totalLight += computeSpotLight(l, spotShadowIndex, worldPos, normal);
                spotShadowIndex++;
            } else {
                totalLight += computeSpotLight(l, -1, worldPos, normal);
            }
        }
    }

    fragColor = vec4(baseColor.rgb * totalLight, baseColor.a);
    
}