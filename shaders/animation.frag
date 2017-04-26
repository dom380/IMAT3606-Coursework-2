#version 430

uniform struct Material {
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
	float d;
	float Ns;
};

uniform sampler2D texture_diffuse;

struct Light
{
	vec3 pos;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

layout (std140) uniform LightingBlock
{
	Light lights[10];
};

uniform Material material;
uniform int NUM_LIGHTS;

uniform bool animatedCharacter;
uniform bool isTextured;
uniform samplerCube depthMap;
uniform float far_plane;
uniform vec3 viewPos;

in Data
{
	vec3 Normal;
	vec3 FragPos;
	vec2 TexCoord;
} inData;

out vec4 FragColor;



vec3 DiffuseAndSpec(Light light, Material material, vec3 norm, vec3 fragmentPos, vec3 viewPos)
{
	//diffuse term
	vec3 lightDir = normalize(light.pos - fragmentPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * light.diffuse * material.Kd;
	diffuse = clamp(diffuse, 0.0, 1.0);

	//specular
	vec3 viewDirection = normalize(viewPos - fragmentPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	vec3 halfwayDir = normalize(lightDir + viewDirection);
	float spec = pow(max(dot(norm, halfwayDir), 0.0), material.Ns);
	vec3 specular = light.specular * spec * material.Ks;
	specular = clamp(specular, 0.0, 1.0);

	return diffuse + spec;
}

float CalculateShadow(Light light, vec3 fragmentPos)
{
	// Get vector between fragment position and light position
    vec3 fragToLight = fragmentPos - light.pos;

    // Use the fragment to light vector to sample from the depth map    
    float closestDepth = texture(depthMap, fragToLight).r;

    // It is currently in linear range between [0,1]. Let's re-transform it back to original depth value
    closestDepth *= far_plane;

    // Now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);

    // Now test for shadows
    float bias = 0.05; // We use a much larger bias since depth is now in [near_plane, far_plane] range
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}

void main()
{
	vec3 result;

	for (int i = 0; i < NUM_LIGHTS; i++)
	{
		vec3 ambient = lights[i].ambient * material.Ka;
		vec3 diffandspec = DiffuseAndSpec(lights[i], material, inData.Normal, inData.FragPos, viewPos);

		//float shadow = CalculateShadow(lights[i], inData.FragPos);
		//result += (ambient + (1.0 - shadow) * diffandspec);
		result += (ambient + diffandspec);
	}

	FragColor = vec4(result, 1.0);
	FragColor = pow( FragColor, vec4(1.0 / 2.2));
}