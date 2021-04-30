#version 330 core

// basic directional light model
uniform vec4		lightDirection; // direction light comes FROM
uniform vec4		lightDiffuseColour;

in vec2 TexCoord;
in vec3 Normal;

// texture sampler
uniform sampler2D texture_diffuse1;

out vec4 FragColour;

void main()
{
	// make sure light direction vector is unit length (store in L)
	vec4 L = normalize(lightDirection);

	// important to normalise length of normal otherwise shading artefacts occur
	vec3 N = normalize(Normal);
	
	vec4 TextureColour = texture(texture_diffuse1, TexCoord);

	// calculate light intensity per-vertex (Gouraud shading)
	float lightIntensity = dot(L.xyz, N);
	float li = clamp(lightIntensity, 0.0, 1.0);

	//FragColour = lightDiffuseColour * TextureColour * li;
	FragColour = TextureColour;

}