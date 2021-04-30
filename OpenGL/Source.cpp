#include "Includes.h"
#include "EarthScene.h"


// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// Camera settings
//							  width, heigh, near plane, far plane
Camera_settings camera_settings{ 1000, 800, 0.1, 100.0 };

//Timer
Timer timer;

// Instantiate the camera object with basic data
Camera camera(camera_settings, glm::vec3(0.0f, 0.2f, 3.0f));

double lastX = camera_settings.screenWidth / 2.0f;
double lastY = camera_settings.screenHeight / 2.0f;

bool			showEarthQuad = false;
EarthScene		*earthScene = nullptr;

int main()
{
	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 8); //MSAA


	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(camera_settings.screenWidth, camera_settings.screenHeight, "Texture Tool", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Set the callback functions
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//Rendering settings
	glfwSwapInterval(1);		// glfw enable swap interval to match screen v-sync
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE); //Enables face culling
	glFrontFace(GL_CCW);//Specifies which winding order if front facing
	glEnable(GL_FRAMEBUFFER_SRGB);
	//glEnable(GL_MULTISAMPLE); //MSAA

	PrincipleAxes	*principleAxes = new PrincipleAxes();

	////	Shaders - Textures - Models	////
	GLuint phongShader;
	GLuint cubeShader;

	GLSL_ERROR glsl_err = ShaderLoader::createShaderProgram(
		string("Resources\\Shaders\\Phong_shader.vs"),
		string("Resources\\Shaders\\Phong_shader.fs"),
		&phongShader);

	//3D model
	//Model cpuModel ("Resources\\Models\\box.obj");
	Model cpuModel("Resources\\Models\\spaceship\\spaceship.obj");


	//
	// Earth scene
	//
	TexturedQuad	*earthQuad = nullptr;
	TexturedQuad	*texturedQuad = nullptr;
	bool			leftCtrlPressed = false;
	static int selecteditem = -1;
	static int selectedmodel = -1;
	static int selectedTexture = -1;
	bool buttonclick1 = false;
	bool buttonclick2 = false;

	earthScene = new EarthScene();
	texturedQuad = new TexturedQuad(string("Resources\\Models\\bumblebee.png"));
	earthQuad = new TexturedQuad(earthScene->getEarthSceneTexture(), true);


	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui::StyleColorsDark();
	ImGui_ImplOpenGL3_Init((char*)glGetString(330));


	static float f = 0.0f;
	static int counter = 0;
	const char* items[] = { "Front", "Top", "Side" };
	const char* models[] = { "Teapot", "Bed", "Sphere", "Box" };
	const char* textures[] = { "Bricks", "Grass", "Water", "Metal" };


	bool show_demo_window = true;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		// input
		processInput(window);
		timer.tick();

		// This demo performs 2 rendering passes...
		// Pass 1) Render the Earth scene to a texture
		// Pass 2) Render the basic demo scene with the principle axes and textured quad, where the texture on the quad is the Earth scene rendered in pass 1.
		//GLuint ModelTexture = TextureLoader::loadTexture(string("Resources\\Models\\Wall.jpg"), TextureGenProperties(GL_SRGB8_ALPHA8));

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();



		//
		// Pass 1. Render the Earth scene
		//
		earthScene->render(selecteditem, buttonclick1, buttonclick2, selectedmodel, selectedTexture);



		//
		// Pass 2. Render the basic demo scene to the screen
		//

		// Clear the screen
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Reset the viewport
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		glm::mat4 model = glm::scale(glm::mat4(1.0), glm::vec3(0.28, 0.28, 0.28));
		glm::mat4 view = camera.getViewMatrix();
		glm::mat4 projection = camera.getProjectionMatrix();

		glm::mat4 viewProjection = projection * view;
		principleAxes->render(viewProjection);

		// Update earthScene state


			if (earthQuad)
				earthQuad->render(glm::mat4(1.0));



			// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
			{

				ImGui::Begin("3D texture tool!");                         

				ImGui::Combo("Model", &selectedmodel, models, IM_ARRAYSIZE(models));
				ImGui::Combo("Texture", &selectedTexture, textures, IM_ARRAYSIZE(textures));
				if (ImGui::Button("Render box"))                          
				{
					cout << "button2 clicked" << endl;
					//cout << "selected = " << selecteditem << endl;
					//cout << "tw = " << earthScene.textureWrapper.planerAxis << endl;
					//cout << "tw after = " << earthScene.textureWrapper.planerAxis << endl;
					//cout << "planer = " << texturewrapper->planerAxis << endl;
					buttonclick2 = true;
					earthScene->render(selecteditem, buttonclick1, buttonclick2, selectedmodel, selectedTexture);
					buttonclick2 = false;
				}
				ImGui::Combo("Texture View", &selecteditem, items, IM_ARRAYSIZE(items));
				if (ImGui::Button("Render planer"))                            
				{
					cout << "button clicked" << endl;
					//cout << "selected = " << selecteditem << endl;
					//cout << "tw = " << earthScene.textureWrapper.planerAxis << endl;
					//cout << "tw after = " << earthScene.textureWrapper.planerAxis << endl;
					//cout << "planer = " << texturewrapper->planerAxis << endl;

					buttonclick1 = true;
					earthScene->render(selecteditem, buttonclick1, buttonclick2, selectedmodel, selectedTexture);
					buttonclick1 = false;
				}

				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
				ImGui::End();
			}
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		
		

		// glfw: swap buffers and poll events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{
	timer.updateDeltaTime();
	Camera *ecam = earthScene->getEarthSceneCamera();

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		ecam->processKeyboard(FORWARD, timer.getDeltaTimeSeconds());
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		ecam->processKeyboard(BACKWARD, timer.getDeltaTimeSeconds());
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		ecam->processKeyboard(LEFT, timer.getDeltaTimeSeconds());
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		ecam->processKeyboard(RIGHT, timer.getDeltaTimeSeconds());
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		showEarthQuad = !showEarthQuad;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	glViewport(0, 0, width, height);
	Camera *ecam = earthScene->getEarthSceneCamera();
	ecam->updateScreenSize(width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		Camera *ecam = earthScene->getEarthSceneCamera();
		ecam->processMouseMovement(xoffset, yoffset);
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		Camera *ecam = earthScene->getEarthSceneCamera();
		ecam->processMouseMovement(xoffset, yoffset);
	}

}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		Camera *ecam = earthScene->getEarthSceneCamera();
		ecam->processMouseScroll(yoffset);
	}
	else
	{
		Camera *ecam = earthScene->getEarthSceneCamera();
		ecam->processMouseScroll(yoffset);
	}
}


// make drop down box with different models and different textures.
//do a questionare for report.