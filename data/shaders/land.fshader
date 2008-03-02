uniform sampler2DShadow shadow;
uniform sampler2D mainmap;
uniform sampler2D detailmap;

varying vec3 normal,lightDir;
//varying vec3 halfVector;

void main()
{
    // Look up the diffuse color and shadow states for each light source.
    float s0 = shadow2DProj(shadow, gl_TexCoord[1]).r;

	// compute the dot product between normal and normalized lightdir
	vec3 n = normalize(normal);
	float NdotL = max(dot(n,normalize(lightDir)),0.0) * s0;

	// add exp fog (depth cueing)
	// float fog = exp2(-gl_Fog.density * gl_FogFragCoord * 7.0 * 1.442695);
	// float fog_factor = clamp(fog, 0.8, 1.0);

	// Light color
	vec4 ambient = gl_LightSource[1].ambient; //* gl_FrontMaterial.ambient;
	vec4 diffuse = gl_LightSource[1].diffuse; //* gl_FrontMaterial.diffuse;
	vec4 lightcolor = (diffuse * NdotL + ambient);

	/* Specular	
	vec3 halfV = normalize(halfVector);
	float NdotHV = max(dot(n,halfV),0.0);
	color += gl_FrontMaterial.specular * gl_LightSource[1].specular * 
					pow(NdotHV,gl_FrontMaterial.shininess);
	*/

    // Compute the final pixel color
	vec4 groundColor = texture2D(mainmap, gl_TexCoord[0].xy);
	vec4 detailColor = texture2D(detailmap, gl_TexCoord[2].xy);
	
	vec3 finalColor =
		((groundColor.rgb * 3.5) + detailColor.rgb) / 4.0 * lightcolor.rgb;
		
	// gl_FragColor = vec4(mix(vec3(0.1, 0.1, 0.1), finalColor, fog_factor), 1.0);
	gl_FragColor = vec4(finalColor, 1.0);
}
