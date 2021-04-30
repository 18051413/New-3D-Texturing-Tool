#include "EarthScene.h"



using namespace std;

TextureWrapper* texturewrapper = new TextureWrapper();


EarthScene::EarthScene() {

	// Camera settings
	//							  width, heigh, near plane, far plane
	Camera_settings camera_settings{ 1000, 800, 0.1, 100.0 };

	// Instanciate the camera object with basic data
	earthCamera = new Camera(camera_settings, glm::vec3(0.0, 5.0, 5.0));

	//
	// Setup textures for rendering the Earth model
	//
	TeaPot_model = new Model("Resources\\Models\\TPot.obj");
	Test_model = new Model("Resources\\Models\\Bed.obj");
	Sphere_model = new Model("Resources\\Models\\spherebigbig.obj");
	Box_model = new Model("Resources\\Models\\cubebigbig.obj");
	cout << "planer = " << texturewrapper->planerAxis << endl;
	
	texturewrapper->planerAxis = Selected;
	cout << "planer = " << texturewrapper->planerAxis << endl;
	texturewrapper->calculatePlanerCoords(&TeaPot_model->meshes[0]);
	BrickTexture = TextureLoader::loadTexture(string("Resources\\Models\\Wall.jpg"), TextureGenProperties(GL_SRGB8_ALPHA8));
	////https ://freestocktextures.com/texture/red-brick-wall-2,855.html
	GrassTexture = TextureLoader::loadTexture(string("Resources\\Models\\Grass.jpg"), TextureGenProperties(GL_SRGB8_ALPHA8));
	////https ://unsplash.com/photos/-laN0yECGm0
	WaterTexture = TextureLoader::loadTexture(string("Resources\\Models\\Water.jpg"), TextureGenProperties(GL_SRGB8_ALPHA8));
	////https ://unsplash.com/photos/5-WsIPUwhlI
	MetalTexture = TextureLoader::loadTexture(string("Resources\\Models\\Metal.jpg"), TextureGenProperties(GL_SRGB8_ALPHA8));
	////https ://unsplash.com/photos/oVFRll_Kp6Q


	GLSL_ERROR glsl_err = ShaderLoader::createShaderProgram(
		string("Resources\\Shaders\\Earth-multitexture.vs"),
		string("Resources\\Shaders\\Earth-multitexture.fs"),
		&earthShader);

	glsl_err = ShaderLoader::createShaderProgram(
		string("Resources\\Shaders\\Phong_shader.vs"),
		string("Resources\\Shaders\\Phong_shader.fs"),
		&phongShader);

	glsl_err = ShaderLoader::createShaderProgram(
		string("Resources\\Shaders\\Cube_shader.vs"),
		string("Resources\\Shaders\\Cube_shader.fs"),
		&cubeShader);

	// Setup uniform locations for shader


	modelMatrixLocation = glGetUniformLocation(earthShader, "modelMatrix");

	invTransposeMatrixLocation = glGetUniformLocation(earthShader, "invTransposeModelMatrix");

	viewProjectionMatrixLocation = glGetUniformLocation(earthShader, "viewProjectionMatrix");

	lightDirectionLocation = glGetUniformLocation(earthShader, "lightDirection");

	lightDiffuseLocation = glGetUniformLocation(earthShader, "lightDiffuseColour");

	lightSpecularLocation = glGetUniformLocation(earthShader, "lightSpecularColour");

	lightSpecExpLocation = glGetUniformLocation(earthShader, "lightSpecularExponent");

	cameraPosLocation = glGetUniformLocation(earthShader, "cameraPos");


	// Set constant uniform data (uniforms that will not change while the application is running)
	// Note: Remember we need to bind the shader before we can set uniform variables!
	glUseProgram(earthShader);

	glUniform1i(dayTextureUniform, 0);
	glUniform1i(nightTextureUniform, 1);
	glUniform1i(maskTextureUniform, 2);

	glUseProgram(0);


	//
	// Setup FBO (which Earth rendering pass will draw into)
	//

	glGenFramebuffers(1, &demoFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, demoFBO);


	//
	// Setup textures that will be drawn into through the FBO
	//

	// Setup colour buffer texture.
	// Note:  The texture is stored as linear RGB values (GL_RGBA8).  
	//There is no need to pass a pointer to image data - 
	//we're going to fill in the image when we render the Earth scene at render time!
	glGenTextures(1, &fboColourTexture);
	glBindTexture(GL_TEXTURE_2D, fboColourTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 800, 800, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	// Setup depth texture

	glGenTextures(1, &fboDepthTexture);
	glBindTexture(GL_TEXTURE_2D, fboDepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, 800, 800, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	//
	// Attach textures to the FBO
	//

	// Attach the colour texture object to the framebuffer object's colour attachment point #0
	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D,
		fboColourTexture,
		0);

	// Attach the depth texture object to the framebuffer object's depth attachment point
	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D,
		fboDepthTexture,
		0);

	//
	// Before proceeding make sure FBO can be used for rendering
	//

	GLenum demoFBOStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (demoFBOStatus != GL_FRAMEBUFFER_COMPLETE) {

		fboOkay = false;
		cout << "Could not successfully create framebuffer object to render texture!" << endl;

	}
	else {

		fboOkay = true;
		cout << "FBO successfully created" << endl;
	}

	// Unbind FBO for now! (Plug main framebuffer back in as rendering destination)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//
	// Setup demo / animation variables
	//

	sunTheta = 0.0f;
	earthTheta = 0.0f;

	// Setup VAO for pyramid object
	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);

	// Setup VBO for vertex position data
	glGenBuffers(1, &cubeVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr); // attribute 0 gets data from bound VBO (so assign vertex position buffer to attribute 0)

	// Setup VBO for vertex colour data
	glGenBuffers(1, &cubeColourBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, cubeColourBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeColours), cubeColours, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, 0, nullptr); // attribute 1 gets colour data

	// Enable vertex position and colour attribute arrays
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	// Setup VBO for face index array
	glGenBuffers(1, &cubeIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeVertexIndices), cubeVertexIndices, GL_STATIC_DRAW);

	// Unbind cube VAO (or bind another VAO for another object / effect)
	// If we didn't do this, we may alter the bindings created above.
	glBindVertexArray(0);
}

EarthScene::~EarthScene() {

}
// Accessor methods

Camera* EarthScene::getEarthSceneCamera() {

	return earthCamera;
}

GLuint EarthScene::getEarthSceneTexture() {

	return fboColourTexture;
}

float EarthScene::getSunTheta() {

	return sunTheta;
}

void EarthScene::updateSunTheta(float thetaDelta) {

	sunTheta += thetaDelta;
}

// Scene update
void EarthScene::update(const float timeDelta) {

	// Update rotation angle ready for next frame
	earthTheta += 15.0f * float(timeDelta);
}

// Rendering methods

void EarthScene::render(int texture, bool buttonclicked, bool buttonclicked2, int chosenmodel, int selTexture) {

	Selected = texture;
	SelectedTexture = BrickTexture;
	if (buttonclicked == true)
	{
		texturewrapper->planerAxis = Selected;
		int noMeshs = SelectedModel->meshes.size();
		for (int i = 0; i < noMeshs; i++)
		{
			texturewrapper->calculatePlanerCoords(&SelectedModel->meshes[i]);
		}
		buttonclicked = false;
	}
	if (buttonclicked2 == true)
	{
		int noMeshs = SelectedModel->meshes.size();
		cout << "num meshes: " << noMeshs << endl;
		for (int i = 0; i < noMeshs; i++)
		{
			texturewrapper->calculateBoxCoords(&SelectedModel->meshes[i]);
		}
		buttonclicked2 = false;
	}
	
	if (!fboOkay)
		return; // Don't render anything if the FBO was not created successfully

	// Bind framebuffer object so all rendering redirected to attached images (i.e. our texture)
	glBindFramebuffer(GL_FRAMEBUFFER, demoFBO);

	// All rendering from this point goes to the bound textures (setup at initialisation time) and NOT the actual screen!!!!!

	// Clear the screen (i.e. the texture)
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set viewport to specified texture size (see above)
	glViewport(0, 0, 800, 800);
	float Rotate = 90;
	glUseProgram(phongShader);
	// Get view-projection transform as a CGMatrix4


	glm::mat4 T = earthCamera->getProjectionMatrix() * earthCamera->getViewMatrix();
	glm::mat4 view = earthCamera->getViewMatrix();
	glm::mat4 projection = earthCamera->getProjectionMatrix();
	glm::mat4 viewProjection = projection * view;

	glm::mat4 model = glm::translate(glm::mat4(1.0), glm::vec3(3.0, 0.0, 0.0));
	glm::mat4 TeaPot = glm::translate(glm::mat4(1.0), glm::vec3(1.8, 1.0, -5.0))*glm::scale(glm::mat4(1.0), glm::vec3(0.04, 0.03, 0.04));

	if (selTexture == 0)
	{
		SelectedTexture = BrickTexture;
	}
	if (selTexture == 1)
	{
		SelectedTexture = GrassTexture;
	}
	if (selTexture == 2)
	{
		SelectedTexture = WaterTexture;
	}
	if (selTexture == 3)
	{
		SelectedTexture = MetalTexture;
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, SelectedTexture);

	//cout << "here" << endl;
	glUniformMatrix4fv(glGetUniformLocation(phongShader, "model"), 1, GL_FALSE, glm::value_ptr(TeaPot));
	glUniformMatrix4fv(glGetUniformLocation(phongShader, "viewProjection"), 1, GL_FALSE, glm::value_ptr(viewProjection));
	//TeaPot_model->draw(phongShader);
	
	//cout << "Model chosen: " << chosenmodel << endl;
	if (chosenmodel == 0)
	{
		SelectedModel = TeaPot_model;
		TeaPot_model->draw(phongShader);
	}
	if (chosenmodel == 1)
	{
		SelectedModel = Test_model;
		Test_model->draw(phongShader);
	}
	if (chosenmodel == 2)
	{
		SelectedModel = Sphere_model;
		Sphere_model->draw(phongShader);
	}
	if (chosenmodel == 3)
	{
		SelectedModel = Box_model;
		Box_model->draw(phongShader);
	}
	//TeaPot_texture->render(TeaPot);
	





	glUseProgram(cubeShader);
	model = glm::translate(glm::mat4(1.0), glm::vec3(2.0, 0.5, -5.0));
	glUniformMatrix4fv(glGetUniformLocation(cubeShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(cubeShader, "viewProjection"), 1, GL_FALSE, glm::value_ptr(viewProjection));
	// Bind VAO that contains all relevant cube VBO buffer and attribute pointer bindings
	glBindVertexArray(cubeVAO);
	 //Draw cube
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, nullptr);

	// Set OpenGL to render to the MAIN framebuffer (ie. the screen itself!!)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
