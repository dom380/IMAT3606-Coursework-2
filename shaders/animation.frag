#version 430

uniform struct MaterialInfo {
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float Shininess;
	sampler2D texture_diffuse;
} Material;

//uniform sampler2DShadow ShadowMap;
//uniform vec3 LightIntensity;

in vec3 Position;
in vec3 Normal;
in vec2 texCoords;
in vec3 outCol;
//in vec4 ShadowCoord;
in vec3 LightPos;

layout (location = 0) out vec4 FragColor;

uniform bool animatedCharacter;

vec3 phongModelDiffAndSpec()
{
	if (animatedCharacter)
	{
		vec3 color = outCol.rgb;

		vec3 n = Normal;
	   if( !gl_FrontFacing ) n = -n;
		vec3 s = normalize(vec3(LightPos) - Position);
		vec3 v = normalize(-Position.xyz);
		vec3 r = reflect( -s, n );
		float sDotN = max( dot(s,n), 0.0 );
		vec3 diffuse = vec3(0.5,0.5,0.5) * color * sDotN;
		vec3 spec = vec3(0.0);
		if( sDotN > 0.0 )
			spec = vec3(0.5,0.5,0.5) * Material.Ks *
				pow( max( dot(r,v), 0.0 ), Material.Shininess );

		return diffuse + spec;	
	}
	else
	{
		vec3 n = Normal;
	   if( !gl_FrontFacing ) n = -n;
		vec3 s = normalize(vec3(LightPos) - Position);
		vec3 v = normalize(-Position.xyz);
		vec3 r = reflect( -s, n );
		float sDotN = max( dot(s,n), 0.0 );
		vec3 diffuse = vec3(0.5,0.5,0.5) * Material.Kd * sDotN;
		vec3 spec = vec3(0.0);
		if( sDotN > 0.0 )
			spec = vec3(0.5,0.5,0.5) * Material.Ks *
				pow( max( dot(r,v), 0.0 ), Material.Shininess );

		return diffuse + spec;
	}
}

void main() {

	vec3 ambient = vec3(0.5,0.5,0.5) * Material.Ka;
	vec3 diffAndSpec = phongModelDiffAndSpec();

   if (animatedCharacter)
	{
		FragColor = vec4(diffAndSpec * outCol.rgb, 1.0);
	}
	else
	{
		FragColor = vec4(diffAndSpec * ambient, 1.0);
	}

	// Gamma correct
    //FragColor = pow( FragColor, vec4(1.0 / 2.2) );   
}
