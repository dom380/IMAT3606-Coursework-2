#version 430

struct Material {
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float d;
	float Ns;
	bool used;
};

struct Light
{
	vec3 pos;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform sampler2D tex;

layout (std140) uniform LightingBlock
{
	Light lights[10];
};

uniform Material material;
uniform int NUM_LIGHTS;

in Data
{
	vec3 Normal;
	vec3 FragPos;
	vec2 texCoord;
	vec3 Ka;
	vec3 Kd;
	float Shininess;
} inData;

uniform vec3 viewPos;
uniform bool isTextured;
out vec4 FragColor;

vec3 calcLight(in Light light, in vec3 Ka, in vec3 Kd, in vec3 Ks, in float Shininess, in vec3 norm, in vec3 fragPos, in vec3 viewDir)
{
	//ambient term
	vec3 ambient = light.ambient * Ka;

	//diffuse term
	vec3 lightDir = normalize(light.pos - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * light.diffuse * Kd;
	diffuse = clamp(diffuse, 0.0, 1.0);

	//specular
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(norm, reflectDir), 0.0), 32);
	vec3 specular = light.specular * spec * Ks;
	specular = clamp(specular, 0.0, 1.0);

	//attenuation
	/*float distance = length(light.pos - fragPos);
	float attenuation = 1.0f / (1.0f + 0.09f * distance + 0.032f * (distance * distance));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;*/

	return vec3(ambient + diffuse + specular);
}

void main() 
{
	vec4 texel = texture(tex, inData.texCoord);


	if (isTextured)
	{
	if(texel.a < 0.3) //Discard any low alpha value fragments as a quick fix for transparency with depth testing 
		discard;
	}
	else
	{
		texel = vec4(1.0);
	}

	vec3 viewDir = normalize(viewPos - inData.FragPos);
	vec3 Ka; vec3 Kd; vec3 Ks; float Shininess;

	if (material.used)
	{
		Ka = material.Ka;
		Kd = material.Kd;
		Ks = material.Ks;
		Shininess = material.Ns;
	}
	else
	{
		Ka = inData.Ka;
		Kd = inData.Kd;
		Ks = inData.Kd;
		Shininess = inData.Shininess;
	}

	vec3 result = vec3(0,0,0);
	for (int i = 0; i < NUM_LIGHTS; i++)
	{
		result += calcLight(lights[i], Ka, Kd, Ks, Shininess, inData.Normal, inData.FragPos, viewDir);
	}

	FragColor = texel * vec4(result, 1.0);

	// Gamma correct
    //FragColor = pow(FragColor, vec4(1.0 / 2.2) );   
}
