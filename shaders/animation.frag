#version 430

uniform struct MaterialInfo {
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float Shininess;
	sampler2D texture_diffuse;
} Material;

uniform sampler2DShadow ShadowMap;
uniform vec3 LightIntensity;

in vec3 Position;
in vec3 Normal;
in vec2 texCoords;
in vec3 outCol;
in vec4 ShadowCoord;
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
		vec3 diffuse = LightIntensity * color * sDotN;
		vec3 spec = vec3(0.0);
		if( sDotN > 0.0 )
			spec = LightIntensity * Material.Ks *
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
		vec3 diffuse = LightIntensity * Material.Kd * sDotN;
		vec3 spec = vec3(0.0);
		if( sDotN > 0.0 )
			spec = LightIntensity * Material.Ks *
				pow( max( dot(r,v), 0.0 ), Material.Shininess );

		return diffuse + spec;
	}
}

subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass;

subroutine (RenderPassType)
void shadeWithShadow()
{
   vec3 ambient = LightIntensity * Material.Ka;
   vec3 diffAndSpec = phongModelDiffAndSpec();

   //float shadow = textureProj(ShadowMap, ShadowCoord);

   //pcf edge softening
   float total = 0;
   total += textureProjOffset(ShadowMap, ShadowCoord, ivec2(-1,-1));
   total += textureProjOffset(ShadowMap, ShadowCoord, ivec2(-1,1));
   total += textureProjOffset(ShadowMap, ShadowCoord, ivec2(1,1));
   total += textureProjOffset(ShadowMap, ShadowCoord, ivec2(1,-1));
   float shadow = total * 0.5;

    // If the fragment is in shadow, use ambient light only.

	if (animatedCharacter)
	{
	FragColor = vec4(diffAndSpec * shadow + outCol.rgb, 1.0);
	}
	else
	{
		FragColor = vec4(diffAndSpec * shadow + ambient, 1.0);
	}

    // Gamma correct
    FragColor = pow( FragColor, vec4(1.0 / 2.2) );   
}

subroutine (RenderPassType)
void recordDepth()
{
    // Outputs gl_FragCoord.z but no code needed as depth is written automatically by Frag Shaders
}

void main() {
    // This will call either shadeWithShadow or recordDepth
    RenderPass();
}
