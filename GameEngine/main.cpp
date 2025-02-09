#include "Graphics\window.h"
#include "Camera\camera.h"
#include "Shaders\shader.h"
#include "Model Loading\mesh.h"
#include "Model Loading\texture.h"
#include "Model Loading\meshLoaderObj.h"
#include <vector>
#include <string>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 

bool showSettingsWindow = false;
float volume = 0.5f; // Default volume
float fov = 90.0f; // Default field of view
int resolutionIndex = 0; // Index for the selected resolution
bool vsyncEnabled = false;

const char* resolutions[] = { "800x600", "1024x768", "1280x720", "1920x1080", "2560x1440" };


void ShowSettingsWindow()
{
	if (showSettingsWindow)
	{
		ImGui::SetCursorPos(ImVec2(300, 40));
		ImGui::Begin("Settings", &showSettingsWindow);

		// Slider for volume
		ImGui::SliderFloat("Volume", &volume, 0.0f, 1.0f);

		// Slider for FOV
		ImGui::SliderFloat("Field of View", &fov, 60.0f, 120.0f);

		// Dropdown list for resolution
		ImGui::Combo("Resolution", &resolutionIndex, resolutions, IM_ARRAYSIZE(resolutions));

		// Checkbox for VSync
		ImGui::Checkbox("VSync", &vsyncEnabled);

		ImGui::End();
	}
}

int currentQuest = 0; // 0 means no quest is selected

void ShowQuestDetails(int questNumber)
{
	switch (questNumber)
	{
	case 1:
		ImGui::Text("Defeat 20 Droids");
		break;
	case 2:
		ImGui::Text("Turn off the production by hitting the power button");
		break;
	case 3:
		ImGui::Text("Pick up the RailGun");
		break;
	case 4:
		ImGui::Text("Defeat 10 Droids using the RailGun");
		break;
	case 5:
		ImGui::Text("Kill Megabot");
		break;
	default:
		break;
	}
}

void ShowQuestWindow()
{
	ImGui::SetCursorPos(ImVec2(0, 0));
	ImGui::Begin("Quests");

	// Buttons for each quest
	if (ImGui::Button("Quest 1")) currentQuest = 1;
	if (ImGui::Button("Quest 2")) currentQuest = 2;
	if (ImGui::Button("Quest 3")) currentQuest = 3;
	if (ImGui::Button("Quest 4")) currentQuest = 4;
	if (ImGui::Button("Quest 5")) currentQuest = 5;

	// Display the details of the selected quest
	ShowQuestDetails(currentQuest);

	ImGui::End();
}






void processKeyboardInput();



float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;



Window window("Game Engine", 800, 800);
Camera camera;

glm::vec3 lightColor = glm::vec3(1.0f);
glm::vec3 lightPos = glm::vec3(-180.0f, 100.0f, -200.0f);

int main()
{
	glClearColor(0.2f, 0.8f, 1.0f, 1.0f);

	//building and compiling shader program
	Shader shader("Shaders/vertex_shader.glsl", "Shaders/fragment_shader.glsl");
	Shader sunShader("Shaders/sun_vertex_shader.glsl", "Shaders/sun_fragment_shader.glsl");
	Shader skyboxShader("Shaders/cube_vertex_shader.glsl", "Shaders/cube_fragment_shader.glsl");
	std::vector<std::string> faces
	{
		"Resources/Textures/Cubemap/skybox/right.jpg",
			"Resources/Textures/Cubemap/skybox/left.jpg",
			"Resources/Textures/Cubemap/skybox/top.jpg",
			"Resources/Textures/Cubemap/skybox/bottom.jpg",
			"Resources/Textures/Cubemap/skybox/front.jpg",
			"Resources/Textures/Cubemap/skybox/back.jpg"
	};
	unsigned int cubemapTexture = loadCubemap(faces);
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


	//Textures
	GLuint tex = loadBMP("Resources/Textures/wood.bmp");
	GLuint tex2 = loadBMP("Resources/Textures/rock.bmp");
	GLuint tex3 = loadBMP("Resources/Textures/orange.bmp");
	GLuint tex4 = loadBMP("Resources/Textures/med_brick.bmp");
	GLuint tex5 = loadBMP("Resources/Textures/water.bmp");
	GLuint tex6 = loadBMP("Resources/Textures/Glock.bmp");
	GLuint tex7 = loadBMP("Resources/Textures/Heicopter.bmp");
	GLuint tex8 = loadBMP("Resources/Textures/sand.bmp");
	glEnable(GL_DEPTH_TEST);

	//Test custom mesh loading
	std::vector<Vertex> vert;
	vert.push_back(Vertex());
	vert[0].pos = glm::vec3(10.5f, 10.5f, 0.0f);
	vert[0].textureCoords = glm::vec2(1.0f, 1.0f);

	vert.push_back(Vertex());
	vert[1].pos = glm::vec3(10.5f, -10.5f, 0.0f);
	vert[1].textureCoords = glm::vec2(1.0f, 0.0f);

	vert.push_back(Vertex());
	vert[2].pos = glm::vec3(-10.5f, -10.5f, 0.0f);
	vert[2].textureCoords = glm::vec2(0.0f, 0.0f);

	vert.push_back(Vertex());
	vert[3].pos = glm::vec3(-10.5f, 10.5f, 0.0f);
	vert[3].textureCoords = glm::vec2(0.0f, 1.0f);

	vert[0].normals = glm::normalize(glm::cross(vert[1].pos - vert[0].pos, vert[3].pos - vert[0].pos));
	vert[1].normals = glm::normalize(glm::cross(vert[2].pos - vert[1].pos, vert[0].pos - vert[1].pos));
	vert[2].normals = glm::normalize(glm::cross(vert[3].pos - vert[2].pos, vert[1].pos - vert[2].pos));
	vert[3].normals = glm::normalize(glm::cross(vert[0].pos - vert[3].pos, vert[2].pos - vert[3].pos));

	std::vector<int> ind = { 0, 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
		1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };

	std::vector<Texture> textures;
	textures.push_back(Texture());
	textures[0].id = tex;
	textures[0].type = "texture_diffuse";

	std::vector<Texture> textures2;
	textures2.push_back(Texture());
	textures2[0].id = tex2;
	textures2[0].type = "texture_diffuse";

	std::vector<Texture> textures3;
	textures3.push_back(Texture());
	textures3[0].id = tex3;
	textures3[0].type = "texture_diffuse";


	std::vector<Texture> textures4;
	textures4.push_back(Texture());
	textures4[0].id = tex4;
	textures4[0].type = "texture_diffuse";

	std::vector<Texture> textures5;
	textures5.push_back(Texture());
	textures5[0].id = tex5;
	textures5[0].type = "texture_diffuse";


	std::vector<Texture> textures6;
	textures6.push_back(Texture());
	textures6[0].id = tex6;
	textures6[0].type = "texture_diffuse";

	std::vector<Texture> textures7;
	textures7.push_back(Texture());
	textures7[0].id = tex7;
	textures7[0].type = "texture_diffuse";

	std::vector<Texture> textures8;
	textures8.push_back(Texture());
	textures8[0].id = tex8;
	textures8[0].type = "texture_diffuse";


	Mesh mesh(vert, ind, textures3);

	// Create Obj files - easier :)
	// we can add here our textures :)
	MeshLoaderObj loader;
	Mesh sun = loader.loadObj("Resources/Models/sphere.obj");
	Mesh box = loader.loadObj("Resources/Models/cube.obj", textures);
	Mesh plane = loader.loadObj("Resources/Models/plane.obj", textures8);
	Mesh helicopter = loader.loadObj("Resources/Models/Helicopter.obj", textures7);
	Mesh house = loader.loadObj("Resources/Models/house.obj", textures4);
	Mesh glock = loader.loadObj("Resources/Models/GLOCK19.obj", textures6);
	Mesh robot = loader.loadObj("Resources/Models/Robot.obj", textures6);
	Mesh rubble = loader.loadObj("Resources/Models/Rubble.obj", textures4);
	Mesh wall = loader.loadObj("Resources/Models/3 piece Wall.obj", textures4);

	//check if we close the window or press the escape button
	while (!window.isPressed(GLFW_KEY_ESCAPE) &&
		glfwWindowShouldClose(window.getWindow()) == 0)
	{

		window.clear();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//ImGui::ShowDemoWindow();
		
		

		/*
		ImGui::Begin("Quest 1");
		ImGui::Text("Defeat 20 Droids");
		ImGui::End();

		ImGui::Begin("Quest 2");
		ImGui::Text("Turn off the production by hitting the power button");
		ImGui::End();

		ImGui::Begin("Quest 3");
		ImGui::Text("Pick up the RailGun");
		ImGui::End();

		ImGui::Begin("Quest 4");
		ImGui::Text("Defeat 10 Droids using the RailGun");
		ImGui::End();

		ImGui::Begin("Quest 5");
		ImGui::Text("Kill Megabot");
		ImGui::End();
		*/

		ImGui::SetCursorPos(ImVec2(20, 100));
		ImGui::Begin("My Window");

		// Button to open settings window
		if (ImGui::Button("Open Settings"))
		{
			showSettingsWindow = true;
		}

		ImGui::End();

		ShowSettingsWindow();
		
		if (ImGui::Button("My Button"))
		{
			// Button code goes here
		}

		/*		
		ImGui::SetCursorPos(ImVec2(20, 40)); // Position for the first text box
		ImGui::Begin("Quest 1");
		ImGui::Text("Defeat 20 Droids");
		ImGui::End();

		ImGui::SetCursorPos(ImVec2(40, 80)); // Position for the second text box
		ImGui::Begin("Quest 2");
		ImGui::Text("Turn off the production by hitting the power button");
		ImGui::End();

		ImGui::SetCursorPos(ImVec2(60, 120)); // Position for the third text box
		ImGui::Begin("Quest 3");
		ImGui::Text("Pick up the RailGun");
		ImGui::End();

		ImGui::SetCursorPos(ImVec2(80, 160)); // Position for the fourth text box
		ImGui::Begin("Quest 4");
		ImGui::Text("Defeat 10 Droids using the RailGun");
		ImGui::End();

		ImGui::SetCursorPos(ImVec2(100, 200)); // Position for the fifth text box
		ImGui::Begin("Quest 5");
		ImGui::Text("Kill Megabot");
		ImGui::End(); 
		*/

		ShowQuestWindow();





		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processKeyboardInput();

		//test mouse input
		if (window.isMousePressed(GLFW_MOUSE_BUTTON_LEFT))
		{
			std::cout << "Pressing mouse button" << std::endl;
		}
		//// Code for the light ////

		sunShader.use();

		glm::mat4 ProjectionMatrix = glm::perspective(90.0f, window.getWidth() * 1.0f / window.getHeight(), 0.1f, 10000.0f);
		glm::mat4 ViewMatrix = glm::lookAt(camera.getCameraPosition(), camera.getCameraPosition() + camera.getCameraViewDirection(), camera.getCameraUp());

		GLuint MatrixID = glGetUniformLocation(sunShader.getId(), "MVP");

		//Test for one Obj loading = light source

		glm::mat4 ModelMatrix = glm::mat4(1.0);
		ModelMatrix = glm::translate(ModelMatrix, lightPos);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		sun.draw(sunShader);


		shader.use();
		GLuint MatrixID4 = glGetUniformLocation(shader.getId(), "MVP");
		GLuint ModelMatrixID3 = glGetUniformLocation(shader.getId(), "model");

		

		///// Test Obj files for helicopter ////
		shader.use();


		ModelMatrix = glm::mat4(1.0);
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(10.0f, -20.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.015f, 0.015f, 0.015f));
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID4, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID3, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		helicopter.draw(shader);
		//// End code for the light ////


		///// Test Obj files for rubble ////
		shader.use();


		ModelMatrix = glm::mat4(1.0);
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(20.0f, -20.0f, 20.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID4, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID3, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		rubble.draw(shader);


		shader.use();
		GLuint MatrixID5 = glGetUniformLocation(shader.getId(), "MVP");
		GLuint ModelMatrixID4
			= glGetUniformLocation(shader.getId(), "model");

		ModelMatrix = glm::mat4(1.0);
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 20.0f, 0.0f));
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID5, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID4, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		glock.draw(shader);

		///// Test Obj files for box ////

		GLuint MatrixID2 = glGetUniformLocation(shader.getId(), "MVP");
		GLuint ModelMatrixID = glGetUniformLocation(shader.getId(), "model");

		ModelMatrix = glm::mat4(1.0);
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		box.draw(shader);

		///// Test plane Obj file //////


		//plane2

		// Assuming you have a list of positions for your instances
		std::vector<glm::vec3> positions = {
			glm::vec3(180.0f, -20.0f, 0.0f),
			glm::vec3(-180.0f, -20.0f, 0.0f),  // Add more positions as needed
			glm::vec3(0.0f, -20.0f, 145.0f),
			glm::vec3(0.0f, -20.0f, -145.0f),
			glm::vec3(-180.0f, -20.0f, -145.0f),
			glm::vec3(180.0f, -20.0f, 145.0f),
			glm::vec3(180.0f, -20.0f, -145.0f),
			glm::vec3(-180.0f, -20.0f, 145.0f),
			glm::vec3(360.0f, -20.0f, -145.0f),
			glm::vec3(-360.0f, -20.0f, -145.0f),
			glm::vec3(360.0f, -20.0f, 0.0f),
			glm::vec3(-360.0f, -20.0f, 0.0f),
			glm::vec3(360.0f, -20.0f, 145.0f),
			glm::vec3(-360.0f, -20.0f, 145.0f),
			glm::vec3(360.0f, -20.0f, -290.0f),
			glm::vec3(-360.0f, -20.0f, -290.0f),
			glm::vec3(360.0f, -20.0f, 290.0f),
			glm::vec3(-360.0f, -20.0f, 290.0f),
			glm::vec3(0.0f, -20.0f, 290.0f),
			glm::vec3(0.0f, -20.0f, -290.0f),
			glm::vec3(180.0f, -20.0f, 290.0f),
			glm::vec3(-180.0f, -20.0f, 290.0f),
			glm::vec3(180.0f, -20.0f, -290.0f),
			glm::vec3(-180.0f, -20.0f, -290.0f),
			glm::vec3(360.0f, -20.0f, 435.0f),
			glm::vec3(-360.0f, -20.0f, 435.0f),
			glm::vec3(360.0f, -20.0f, -435.0f),
			glm::vec3(-360.0f, -20.0f, -435.0f),
			glm::vec3(0.0f, -20.0f, 435.0f),
			glm::vec3(0.0f, -20.0f, -435.0f),
			glm::vec3(180.0f, -20.0f, 435.0f),
			glm::vec3(-180.0f, -20.0f, 435.0f),
			glm::vec3(180.0f, -20.0f, -435.0f),
			glm::vec3(-180.0f, -20.0f, -435.0f),
			glm::vec3(540.0f, -20.0f, 0.0f),
			glm::vec3(-540.0f, -20.0f, 0.0f),
			glm::vec3(540.0f, -20.0f, 145.0f),
			glm::vec3(-540.0f, -20.0f, 145.0f),
			glm::vec3(540.0f, -20.0f, -145.0f),
			glm::vec3(-540.0f, -20.0f, -145.0f),
			glm::vec3(540.0f, -20.0f, 290.0f),
			glm::vec3(-540.0f, -20.0f, 290.0f),
			glm::vec3(540.0f, -20.0f, -290.0f),
			glm::vec3(-540.0f, -20.0f, -290.0f),
			glm::vec3(540.0f, -20.0f, 435.0f),
			glm::vec3(-540.0f, -20.0f, 435.0f),
			glm::vec3(540.0f, -20.0f, -435.0f),
			glm::vec3(-540.0f, -20.0f, -435.0f),
			glm::vec3(0.0f, -20.0f, 0.0f),
			// ...
		};

		// Loop through each position and render the object at that position
		for (const auto& position : positions) {
			glm::mat4 ModelMatrix = glm::mat4(1.0);
			ModelMatrix = glm::translate(ModelMatrix, position);
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

			plane.draw(shader);
		}
		//Walls
		std::vector<glm::vec3> positions1 = {
			glm::vec3(40.0f, -20.0f, 0.0f),
			glm::vec3(80.0f, -20.0f, -1.0f),

		};
		for (const auto& position : positions1) {
			glm::mat4 ModelMatrix = glm::mat4(1.0);
			ModelMatrix = glm::translate(ModelMatrix, position);
			ModelMatrix = glm::rotate(ModelMatrix, glm::radians(90.0f), glm::vec3(10.0f, 10.0f, 10.0f));
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

			wall.draw(shader);
		}
		 
		
		//droid
		std::vector<glm::vec3> robotPositions = {
	//glm::vec3(10.0f, -20.0f, 0.0f),
	glm::vec3(20.0f, -20.0f, 0.0f),
	glm::vec3(30.0f, -20.0f, 0.0f),
	glm::vec3(40.0f, -20.0f, 0.0f),
	// Add more positions as needed
		};
		float animationSpeed = 2.0f;  // Adjust the animation speed as needed
		float walkCycleDuration = 2.0f;// Adjust the walk cycle duration as needed
		float stepSize = 5.0f;
		for (const auto& robotPosition : robotPositions) {
			glm::mat4 ModelMatrix = glm::mat4(1.0);

			// Animate the walking motion
			float translation = std::sin(glfwGetTime() * animationSpeed) * stepSize;
			float legRotation = std::sin(glfwGetTime() * animationSpeed * 2.0f) * 0.5f; // Adjust rotation speed and angle

			// Translate the droid to the position and add walking translation
			ModelMatrix = glm::translate(ModelMatrix, robotPosition + glm::vec3(translation, 0.0f, 0.0f));

			// Rotate the left leg
			glm::mat4 leftLegRotation = glm::rotate(glm::mat4(1.0), legRotation, glm::vec3(0.0f, 1.0f, 0.0f));
			ModelMatrix = ModelMatrix * leftLegRotation;

			// Set up the MVP matrix and draw
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

			robot.draw(shader);
		}



		glBindVertexArray(0);

		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader.use();
		glm::mat4 view = glm::mat4(glm::mat3(camera.getViewMatrix())); // remove translation from the view matrix
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.getId(), "view"), 1, GL_FALSE, &view[0][0]);

		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.getId(), "projection"), 1, GL_FALSE, &ProjectionMatrix[0][0]);

		glUniform1i(glGetUniformLocation(skyboxShader.getId(), "skybox"), 0);
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default


		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		window.update();
	}
}

void processKeyboardInput()
{
	float cameraSpeed = 30 * deltaTime;

	// Translation
	if (window.isPressed(GLFW_KEY_W))
		camera.keyboardMoveFront(cameraSpeed);
	if (window.isPressed(GLFW_KEY_S))
		camera.keyboardMoveBack(cameraSpeed);
	if (window.isPressed(GLFW_KEY_A))
		camera.keyboardMoveLeft(cameraSpeed);
	if (window.isPressed(GLFW_KEY_D))
		camera.keyboardMoveRight(cameraSpeed);
	if (window.isPressed(GLFW_KEY_R))
		camera.keyboardMoveUp(cameraSpeed);
	if (window.isPressed(GLFW_KEY_F))
		camera.keyboardMoveDown(cameraSpeed);



	//rotation
	if (window.isPressed(GLFW_KEY_LEFT))
		camera.rotateOy(cameraSpeed);
	if (window.isPressed(GLFW_KEY_RIGHT))
		camera.rotateOy(-cameraSpeed);
	if (window.isPressed(GLFW_KEY_UP))
		camera.rotateOx(cameraSpeed);
	if (window.isPressed(GLFW_KEY_DOWN))
		camera.rotateOx(-cameraSpeed);

}