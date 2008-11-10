uniform sampler2DShadow shadow;
uniform sampler2D mainmap;
uniform sampler2D detailmap;

varying vec3 normal,lightDir;

void main()
{
	// add exp fog
	float fog = exp2(-gl_Fog.density * gl_FogFragCoord * 3.0 * 1.442695);
	float fog_factor = clamp(fog, 0.0, 1.0);

    // Look up the diffuse color and shadow states for each light source.
    float s0 = shadow2DProj(shadow, gl_TexCoord[2]).r;

	// compute the dot product between normal and normalized lightdir
	vec3 n = normalize(normal);
	float NdotL = max(dot(n,normalize(lightDir)),0.0) * s0;

	// Light color
	vec4 ambient = gl_LightSource[1].ambient; //* gl_FrontMaterial.ambient;
	vec4 diffuse = gl_LightSource[1].diffuse; //* gl_FrontMaterial.diffuse;
	vec4 lightcolor = (diffuse * NdotL + ambient);

    // Compute the final pixel color
	vec4 groundColor = texture2D(mainmap, gl_TexCoord[0].xy);
	vec4 detailColor = texture2D(detailmap, gl_TexCoord[1].xy);
	
	vec3 finalColor =
		((groundColor.rgb * 3.5) + detailColor.rgb) / 4.0 * lightcolor.rgb;
		
	gl_FragColor = vec4(mix(vec3(gl_Fog.color), finalColor, fog_factor), 1.0);
}
