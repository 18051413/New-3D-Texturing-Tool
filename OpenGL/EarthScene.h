#ifndef EARTH_SCENE_H
#define EARTH_SCENE_H
#include "Model.h"
#include "TextureLoader.h"
#include "Sphere.h"
#include "Camera.h"
#include "ShaderLoader.h"
#include <iostream>
#include "VertexData.h"
#include "Includes.h"
#include "TextureWrapper.h"

class Sphere;

class EarthScene{

private:

	Sphere						*earthModel;

	// Move around the earth with a seperate camera to the main scene camera
	Camera						*earthCamera;

	// Textures for multi-texturing the earth model
	//GLuint							dayTexture;
	//GLuint							nightTexture;
	//GLuint							cloudMaskTexture;

	// Shader for multi-texturing the earth
	GLuint							earthShader;
	GLuint							phongShader;
	GLuint							cubeShader;
	GLuint cubeVertexBuffer;
	GLuint cubeColourBuffer;
	GLuint cubeIndexBuffer;


	TexturedQuad* TeaPot_texture;
	GLuint BrickTexture;
	GLuint GrassTexture;
	GLuint WaterTexture;
	GLuint MetalTexture;


	// Unifom locations for earthShader

	GLuint cubeVAO;

	Model *Floor_model;
	Model *TeaPot_model;
	Model *Test_model;
	Model* Sphere_model;
	Model* Box_model;
	Model* SelectedModel;

	// Texture uniforms
	GLuint							dayTextureUniform;
	GLuint							nightTextureUniform;
	GLuint							maskTextureUniform;

	// Camera uniforms
	GLint							modelMatrixLocation;
	GLint							invTransposeMatrixLocation;
	GLint							viewProjectionMatrixLocation;

	// Directional light uniforms
	GLint							lightDirectionLocation;
	GLint							lightDiffuseLocation;
	GLint							lightSpecularLocation;
	GLint							lightSpecExpLocation;
	GLint							cameraPosLocation;


	//
	// Animation state
	//
	float							sunTheta; // Angle to the Sun in the orbital plane of the Earth (the xz plane in the demo)
	float							earthTheta;


	//
	// Framebuffer Object (FBO) variables
	//

	// Actual FBO
	GLuint							demoFBO;

	// Colour texture to render into
	GLuint							fboColourTexture;

	// Depth texture to render into
	GLuint							fboDepthTexture;

	// Flag to indicate that the FBO is valid
	bool							fboOkay;

public:

	EarthScene();
	~EarthScene();

	int Selected;
	int SelectedTexture;
	bool buttonclicked;
	bool buttonclicked2;

	// Accessor methods
	Camera* getEarthSceneCamera();
	GLuint getEarthSceneTexture();
	float getSunTheta();
	void updateSunTheta(float thetaDelta);

	// Scene update
	void update(const float timeDelta);

	// Rendering methods
	void render(int texture, bool buttonclicked, bool buttonclicked2, int model, int selTexture);
};

#endif