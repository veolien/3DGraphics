#include<iostream>
#include <sstream>
//for debug
#include <fstream>
#include <string>
#include <vector>

#include <map>
//include glad before GLFW to avoid header conflict or define "#define GLFW_INCLUDE_NONE"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>


#include"Camera.h"

//include the stb library, look at their read me
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include"Object.h"
bool tablette::initialized = false;
GLuint tablette::cross_tex;
GLuint tablette::miss_tex;
GLuint tablette::check_tex;
GLuint tablette::choose_tex;

#include <chrono>
#include <thread>

const float width = 1024.0;
const float height = 800.0;

enum modes {
	No_Mode, Ship_Positionning1, Ship_Positionning2, Attack1, Attack2, Win1, Win2, Explosion
};

struct Particle {
	glm::vec3 Position, Velocity;
	glm::vec3 orientation;
	float Degree;
	glm::vec4 Color;
	float     Life;

	Particle()
		: Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) { }
};


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
GLuint compileShader(std::string shaderCode, GLenum shaderType);
GLuint compileProgram(GLuint vertexShader, GLuint fragmentShader);

void SetObject(object obj, camera cam, int u_model, int u_view, int u_perspective, int u_tex);
void SetObject(ship obj, camera cam, int u_model, int u_view, int u_perspective, int u_tex, int u_invmodel, int u_u_view_pos);
void SetObject(tablette obj, camera cam, int u_model, int u_tex, int u_cross_tex, int u_miss_tex, int u_check_tex, int u_choose_tex, int interact);
void SetObject2(object obj, camera cam, int u_model, int u_view, int u_perspective, int u_tex);
void SetObject2(ship obj, camera cam, int u_model, int u_view, int u_perspective, int u_tex, int u_invmodel, int u_u_view_pos);
void SetObject2(tablette obj, camera cam, int u_model, int u_tex, int u_cross_tex, int u_miss_tex, int u_check_tex, int u_choose_tex, int interact);

glm::mat4 SetObject2_Rotate(object obj);
void SetObject_bis(object obj, camera cam, int u_model, int u_view, int u_perspective, int u_tex, glm::mat4 M);
void SetObject_bis(ship obj, camera cam, int u_model, int u_view, int u_perspective, int u_tex, glm::mat4 M, int u_invmodel, int u_u_view_pos);
void SetObject_bis(tablette obj, camera cam, int u_model, int u_tex, glm::mat4 M, int u_cross_tex, int u_miss_tex, int u_check_tex, int u_choose_tex, int interact);
void loadCubemapFace(const char* path, const GLenum& targetFace);

void RespawnParticle(Particle& particle, glm::vec3 explosionOrigin);
void UpdateParticles(float dt, std::vector<Particle>& particles);
void TriggerExplosion(glm::vec3 center, std::vector<Particle>& particles);
unsigned int FirstUnusedParticle(std::vector<Particle> particles);
bool CheckIfEmpty(std::vector<Particle> particles);

#ifndef NDEBUG
void APIENTRY glDebugOutput(GLenum source,
	GLenum type,
	unsigned int id,
	GLenum severity,
	GLsizei length,
	const char* message,
	const void* userParam)
{
	// ignore non-significant error/warning codes
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

	std::cout << "---------------" << std::endl;
	std::cout << "Debug message (" << id << "): " << message << std::endl;

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
	case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
	} std::cout << std::endl;

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
	case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
	} std::cout << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
	case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
	} std::cout << std::endl;
	std::cout << std::endl;
}
#endif

int main(int argc, char* argv[])
{
	std::cout << "Welcome to the final project: " << std::endl;
	std::cout << "Battleship\n";
	//Boilerplate
	//Create the OpenGL context 
	if (!glfwInit()) {
		throw std::runtime_error("Failed to initialise GLFW \n");
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifndef NDEBUG
	//create a debug context to help with Debugging
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif


	//Create the window
	GLFWwindow* window = glfwCreateWindow(width, height, "Final Project, BattleShip", nullptr, nullptr);
	if (window == NULL)
	{
		glfwTerminate();
		throw std::runtime_error("Failed to create GLFW window\n");
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//load openGL function
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		throw std::runtime_error("Failed to initialize GLAD");
	}

#ifndef NDEBUG
	int flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
#endif
	// shaders
	//program 1 ships
	const std::string sourceV = "#version 330 core\n"
		"in vec3 position; \n"
		"in vec2 texcoord; \n"
		"in vec3 normal; \n"

		"out vec2 vtex;"
		"out vec3 v_frag_coord; \n"
		"out vec3 v_normal; \n"
		"uniform mat4 M; \n"
		"uniform mat4 itM; \n"
		"uniform mat4 V; \n"
		"uniform mat4 P; \n"
		" void main(){ \n"
		"vec4 frag_coord = M*vec4(position, 1.0); \n"
		"gl_Position = P*V*frag_coord;\n"
		"v_normal = vec3(itM * vec4(normal, 1.0)); \n"
		"v_frag_coord = frag_coord.xyz; \n"
		"vtex=texcoord;\n"
		"}\n";
	const std::string sourceF = "#version 330 core\n"
		"precision mediump float; \n"
		"in vec2 vtex;\n"
		"in vec3 v_frag_coord; \n"
		"in vec3 v_normal; \n"
		"out vec4 FragColor; \n"


		"uniform sampler2D ourTexture; \n"


		"uniform vec3 u_view_pos; \n"

		"struct Light{\n"
		"vec3 light_pos; \n"
		"float ambient_strength; \n"
		"float diffuse_strength; \n"
		"float specular_strength; \n"
		"float constant;\n"
		"float linear;\n"
		"float quadratic;\n"
		"};\n"

		"uniform Light light;"

		"uniform float shininess; \n"
		"uniform vec3 materialColour; \n"


		"float specularCalculation(vec3 N, vec3 L, vec3 V ){ \n"
		"vec3 R = reflect (-L,N);  \n "
		"float cosTheta = dot(R , V); \n"
		"float spec = pow(max(cosTheta,0.0), 32.0); \n"
		"return light.specular_strength * spec;\n"
		"}\n"

		"void main() { \n"
		"vec3 N = normalize(v_normal);\n"
		"vec3 L = normalize(light.light_pos - v_frag_coord) ; \n"
		"vec3 V = normalize(u_view_pos - v_frag_coord); \n"
		"float specular = specularCalculation( N, L, V); \n"
		"float diffuse = light.diffuse_strength * max(dot(N,L),0.0);\n"
		"float distance = length(light.light_pos - v_frag_coord);"
		"float attenuation = 1 / (light.constant + light.linear * distance + light.quadratic * distance * distance);"
		"float light = light.ambient_strength + attenuation * (diffuse + specular); \n"
		"FragColor = texture(ourTexture, vtex); \n"
		"FragColor = vec4(vec3(FragColor) * vec3(light), 1.0); \n"
		"} \n";

	GLuint shaderV = compileShader(sourceV, GL_VERTEX_SHADER);
	GLuint shaderF = compileShader(sourceF, GL_FRAGMENT_SHADER);
	GLuint program = compileProgram(shaderV, shaderF);
	// program 2 tablette

	const std::string sourceV2 = "#version 330 core\n"
		"in vec3 position; \n"
		"in vec2 texcoord; \n"
		"out vec2 vtex; "
		"uniform mat4 M; \n"
		" void main(){ \n"
		"gl_Position = M*vec4(position, 1);\n"
		"vtex=texcoord;\n"
		"}\n";
	const std::string sourceF2 = "#version 330 core\n"
		"precision mediump float; \n"
		"in vec2 vtex;\n"
		"out vec4 FragColor;"
		"uniform sampler2D ourTexture; \n"
		"uniform sampler2D crossTex; \n"
		"uniform sampler2D missTex; \n"
		"uniform sampler2D checkTex; \n"
		"uniform sampler2D chooseTex; \n"
		"uniform sampler2D interact; \n"
		"void main() { \n"
		"	FragColor = texture(ourTexture, vtex); \n"
		"	vec2 vGrid = vec2((vtex.x*140.0)+130.0,(vtex.y*200.0)-70.0);\n"
		"	vec2 vCase = vec2(float(int(vGrid.x/10))*0.1-0.05,1.1-float(int(vGrid.y/10))*0.1-0.05);\n"
		"	if(vGrid.x < 110.1 && vGrid.x > 9.9 && vGrid.y < 110.1 && vGrid.y > 9.9 ){\n"
		"		float interact_value = texture(interact, vCase).r; \n"
		"		if(int(vGrid.x)%10 < 9 && int(vGrid.x)%10 > 0 && int(vGrid.y)%10 < 9 && int(vGrid.y)%10 > 0){\n"
		"			if(interact_value < 0.4 && interact_value > 0.2){\n"
		"				FragColor = texture(crossTex, vtex);\n"
		"			} \n"
		"			else if (interact_value < 0.6 && interact_value > 0.4) {\n"
		"				FragColor = texture(missTex, vtex);\n"
		"			} \n"
		"			else if (interact_value < 0.8 && interact_value > 0.6) {\n"
		"				FragColor = texture(checkTex, vtex);\n"
		"			} \n"
		"			else if (interact_value < 1.0 && interact_value > 0.8) {\n"
		"				FragColor = texture(chooseTex, vtex);\n"
		"			} \n"
		"		}\n"
		"	}\n"
		"}\n";

	GLuint shaderV2 = compileShader(sourceV2, GL_VERTEX_SHADER);
	GLuint shaderF2 = compileShader(sourceF2, GL_FRAGMENT_SHADER);
	GLuint program2 = compileProgram(shaderV2, shaderF2);

	//program 3 Base
	const std::string sourceV3 = "#version 330 core\n"
		"in vec3 position; \n"
		"in vec2 texcoord; \n"
		"out vec2 vtex;"
		"uniform mat4 M; \n"
		"uniform mat4 V; \n"
		"uniform mat4 P; \n"
		" void main(){ \n"
		"gl_Position = P*V*M*vec4(position, 1);\n"
		"vtex=texcoord;\n"
		"}\n";
	const std::string sourceF3 = "#version 330 core\n"
		"precision mediump float; \n"
		"in vec2 vtex;\n"
		"out vec4 FragColor;"
		"uniform sampler2D ourTexture; \n"
		"void main() { \n"
		"FragColor = texture(ourTexture, vtex); \n"
		"} \n";

	GLuint shaderV3 = compileShader(sourceV3, GL_VERTEX_SHADER);
	GLuint shaderF3 = compileShader(sourceF3, GL_FRAGMENT_SHADER);
	GLuint program3 = compileProgram(shaderV3, shaderF3);

	//program 4 Cube map
	const std::string sourceV4 = "#version 330 core\n"
		"in vec3 position; \n"
		"in vec2 texcoord; \n"
		"in vec3 normal; \n"

		"out vec3 vtex;"
		"uniform mat4 V; \n"
		"uniform mat4 P; \n"
		" void main(){ \n"
		"mat4 V_no_rot = mat4(mat3(V)) ;\n"
		"vec4 pos = P*V_no_rot*vec4(position, 1);\n"
		"gl_Position = pos.xyww;\n"
		"vtex=position;\n"
		"}\n";
	const std::string sourceF4 = "#version 330 core\n"
		"precision mediump float; \n"
		"in vec3 vtex;\n"
		"out vec4 FragColor;"
		"uniform samplerCube cubemapSampler; \n"
		"void main() { \n"
		"FragColor = texture(cubemapSampler, vtex); \n"
		"} \n";

	GLuint shaderV4 = compileShader(sourceV4, GL_VERTEX_SHADER);
	GLuint shaderF4 = compileShader(sourceF4, GL_FRAGMENT_SHADER);
	GLuint program4 = compileProgram(shaderV4, shaderF4);

	// program 5 particles ( explosion )
	const std::string sourceV5 = "#version 330 core\n"
		"in vec3 position; \n"
		"in vec2 texcoord; \n"
		"in vec3 normal; \n"

		"out vec2 vtex;"
		"out vec4 ParticleColor;"

		"uniform mat4 P; \n"
		"uniform mat4 V; \n"
		"uniform vec3 offset; \n"
		"uniform vec4 color; \n"
		"uniform float size; \n"

		" void main(){ \n"
		"vec3 worldPos = offset + position*size;\n"
		"gl_Position = P * V *  vec4(worldPos, 1.0);\n"
		"vtex = texcoord;\n"
		"ParticleColor = color;\n"
		"}\n";
	const std::string sourceF5 = "#version 330 core\n"
		"precision mediump float; \n"

		"in vec4 ParticleColor;\n"
		"in vec2 vtex;\n"

		"out vec4 FragColor;"

		"uniform sampler2D sprite; \n"

		"void main() { \n"
		"FragColor = texture(sprite, vtex)*ParticleColor; \n"
		"} \n";

	GLuint shaderV5 = compileShader(sourceV5, GL_VERTEX_SHADER);
	GLuint shaderF5 = compileShader(sourceF5, GL_FRAGMENT_SHADER);
	GLuint program5 = compileProgram(shaderV5, shaderF5);

	//program 3 Mirror
	const std::string sourceV6 = "#version 330 core\n"
		"in vec3 position; \n"
		"in vec2 tex_coords; \n"
		"in vec3 normal; \n" // normals from the object but are varyied

		"out vec3 v_frag_coord; \n"
		"out vec3 v_normal; \n"

		"uniform mat4 M; \n"
		"uniform mat4 itM; \n" // needed to compute the mirror effect with the normal
		"uniform mat4 V; \n"
		"uniform mat4 P; \n"


		" void main(){ \n"
		"vec4 frag_coord = M*vec4(position, 1.0); \n"
		"gl_Position = P*V*frag_coord; \n"
		"v_normal = vec3(-1.0,0.0,0.0);\n" // hard code the normal because opengl is modifying the compiled normals from the .obj file.
		"v_frag_coord = frag_coord.xyz; \n"
		"\n"
		"}\n";
	const std::string sourceF6 = "#version 400 core\n"
		"out vec4 FragColor;\n"
		"precision mediump float; \n"

		"in vec3 v_frag_coord; \n"
		"in vec3 v_normal; \n"

		"uniform vec3 u_view_pos; \n"
		"uniform samplerCube cubemapSampler; \n"

		"void main() { \n"
		"vec3 N = normalize(v_normal);\n"
		"vec3 V = normalize(u_view_pos - v_frag_coord); \n"
		"vec3 R = reflect(-V,N); \n"
		"R = vec3(R.x, R.y, R.z);\n"
		"FragColor = texture(cubemapSampler,R); \n"
		"} \n";

	GLuint shaderV6 = compileShader(sourceV6, GL_VERTEX_SHADER);
	GLuint shaderF6 = compileShader(sourceF6, GL_FRAGMENT_SHADER);
	GLuint program6 = compileProgram(shaderV6, shaderF6);

	//        ================Objects===========
	// Bases
	base Base_1("BaseV1.obj", "Grid.jpg");
	base Base_2("BaseV1.obj", "Grid.jpg");
	// Ships
	ship Ship4_1("pirate_ship4V1.obj", "BaseColor.jpg", 4);
	ship Ship3_1("pirate_ship4V1.obj", "BaseColor.jpg", 4);
	ship Ship2_1("pirate_ship2V1.obj", "BaseColor.jpg", 2);
	ship Ship4_2("pirate_ship4V1.obj", "BaseColor.jpg", 4);
	ship Ship3_2("pirate_ship4V1.obj", "BaseColor.jpg", 4);
	ship Ship2_2("pirate_ship2V1.obj", "BaseColor.jpg", 2);
	// Tablettes
	tablette Tablette_1("TabletteV1.obj", "Tablette.jpg");
	tablette Tablette_2("TabletteV1.obj", "Tablette.jpg");
	//cubemap
	object cubemap("CubeMap.obj", "BaseColor.jpg");
	GLuint cubeMapTexture;
	glGenTextures(1, &cubeMapTexture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);

	// texture parameters
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_set_flip_vertically_on_load(false);
	std::string pathToCubeMap = "data/NissiBeach/";

	std::map<std::string, GLenum> facesToLoad = {
		{pathToCubeMap + "posx.jpg",GL_TEXTURE_CUBE_MAP_POSITIVE_X},
		{pathToCubeMap + "posy.jpg",GL_TEXTURE_CUBE_MAP_POSITIVE_Y},
		{pathToCubeMap + "posz.jpg",GL_TEXTURE_CUBE_MAP_POSITIVE_Z},
		{pathToCubeMap + "negx.jpg",GL_TEXTURE_CUBE_MAP_NEGATIVE_X},
		{pathToCubeMap + "negy.jpg",GL_TEXTURE_CUBE_MAP_NEGATIVE_Y},
		{pathToCubeMap + "negz.jpg",GL_TEXTURE_CUBE_MAP_NEGATIVE_Z},
	};
	//load the six faces
	for (std::pair<std::string, GLenum> pair : facesToLoad) {
		loadCubemapFace(pair.first.c_str(), pair.second);
	}
	
	//explosion
	std::vector<Particle> particles;
	particles.resize(500); // Capacity
	object particle("particle.obj", "BaseColor.jpg");

	//mirror
	object Mirror("BaseV1.obj", "BaseColor.jpg");

	//fps
	double prev_fps_logging_time = 0;
	int  deltaFrameCount_for_fps_logging = 0;
	//fps function
	auto fps = [&](double current_time_for_fps, camera cam1) {
		double elapsed_since_last_log = current_time_for_fps - prev_fps_logging_time;
		deltaFrameCount_for_fps_logging++;
		if (elapsed_since_last_log > 0.5) {
			prev_fps_logging_time = current_time_for_fps;
			const double fpsCountValue = (double)deltaFrameCount_for_fps_logging / elapsed_since_last_log;
			deltaFrameCount_for_fps_logging = 0;
			// Optional: print FPS
			 std::cout << "\r FPS: " << fpsCountValue;
			 std::cout.flush();
		}
	};


	//Initial positionning

	Tablette_1.set_model(glm::rotate(Tablette_1.get_model(), Pi / 2.0f, glm::vec3(1.0, 0.0, 0.0)));
	Tablette_1.set_model(glm::rotate(Tablette_1.get_model(), Pi / 2.0f, glm::vec3(0.0, 1.0, 0.0)));
	Tablette_1.set_model(glm::translate(Tablette_1.get_model(), glm::vec3(-1.0, 0.0, 1.0)));

	Tablette_2.set_model(glm::rotate(Tablette_2.get_model(), Pi / 2.0f, glm::vec3(1.0, 0.0, 0.0)));
	Tablette_2.set_model(glm::rotate(Tablette_2.get_model(), Pi / 2.0f, glm::vec3(0.0, 1.0, 0.0)));
	Tablette_2.set_model(glm::translate(Tablette_2.get_model(), glm::vec3(-1.0, 0.0, 1.0)));

	//mirror
	Mirror.set_model(glm::rotate(Mirror.get_model(), Pi / 2.0f, glm::vec3(0.0, 0.0, 1.0)));
	Mirror.set_model(glm::translate(Mirror.get_model(), glm::vec3(0.0, 0.0, 15.0)));

	//camera
	glm::vec3 Pos = glm::vec3(-22.0, 15.0, -5.0);
	glm::vec2 AngularDirection = glm::vec2(0, -Pi / 6);
	float fov = 45.0f;
	float AR = width / height;
	float Boundaries[2] = { 0.01,100.0 };
	camera cam1(Pos, AngularDirection, fov, AR, Boundaries);
	glfwSetWindowUserPointer(window, &cam1);


	//uniform
	auto u_model = glGetUniformLocation(program, "M");
	auto u_invmodel = glGetUniformLocation(program, "itM");
	auto u_view = glGetUniformLocation(program, "V");
	auto u_perspective = glGetUniformLocation(program, "P");
	auto u_tex = glGetUniformLocation(program, "ourTexture");
	auto u_u_view_pos = glGetUniformLocation(program, "u_view_pos");
	auto u_shininess = glGetUniformLocation(program, "shininess");
	auto u_ligtPos = glGetUniformLocation(program, "light.light_pos");
	auto u_ambient = glGetUniformLocation(program, "light.ambient_strength");
	auto u_diffuse = glGetUniformLocation(program, "light.diffuse_strength");
	auto u_strength = glGetUniformLocation(program, "light.specular_strength");
	auto u_constant = glGetUniformLocation(program, "light.constant");
	auto u_linear = glGetUniformLocation(program, "light.linear");
	auto u_quadratic = glGetUniformLocation(program, "light.quadratic");


	auto u_model2 = glGetUniformLocation(program2, "M");
	auto u_tex2 = glGetUniformLocation(program2, "ourTexture");
	auto u_cross_tex = glGetUniformLocation(program2, "crossTex");
	auto u_miss_tex = glGetUniformLocation(program2, "missTex");
	auto u_check_tex = glGetUniformLocation(program2, "checkTex");
	auto u_choose_tex = glGetUniformLocation(program2, "chooseTex");
	auto u_interact = glGetUniformLocation(program2, "interact");

	auto u_model3 = glGetUniformLocation(program3, "M");
	auto u_view3 = glGetUniformLocation(program3, "V");
	auto u_perspective3 = glGetUniformLocation(program3, "P");
	auto u_tex3 = glGetUniformLocation(program3, "ourTexture");

	auto u_view4 = glGetUniformLocation(program4, "V");
	auto u_perspective4 = glGetUniformLocation(program4, "P");
	auto u_tex4 = glGetUniformLocation(program4, "cubemapSampler");

	auto u_offset5 = glGetUniformLocation(program5, "offset");
	auto u_color5 = glGetUniformLocation(program5, "color");
	auto u_size5 = glGetUniformLocation(program5, "size");
	auto u_perspective5 = glGetUniformLocation(program5, "P");
	auto u_view5 = glGetUniformLocation(program5, "V");
	auto u_tex5 = glGetUniformLocation(program5, "sprite");

	auto u_model6 = glGetUniformLocation(program6, "M");
	auto u_invmodel6 = glGetUniformLocation(program6, "itM");
	auto u_view6 = glGetUniformLocation(program6, "V");
	auto u_perspective6 = glGetUniformLocation(program6, "P");
	auto u_tex6 = glGetUniformLocation(program6, "cubemapSampler");
	auto u_u_view_pos6 = glGetUniformLocation(program6, "u_view_pos");

	//Light init
	glm::vec3 light_pos = glm::vec3(12.5, 20.0, 0.0);
	glUseProgram(program);
	glUniform3f(u_ligtPos, light_pos.x, light_pos.y, light_pos.z);
	glUniform1f(u_shininess, 32.0f);
	glUniform1f(u_ambient, 0.1);
	glUniform1f(u_diffuse, 6.0);
	glUniform1f(u_strength, 10.0);
	glUniform1f(u_constant, 1.0);
	glUniform1f(u_linear, 0.14);
	glUniform1f(u_quadratic, 0.07);
	
	//sync with the screen refresh rate
	glfwSwapInterval(1);
	//Rendering
	bool display1 = 0;
	bool display2 = 0;
	bool display3 = 0;
	bool display1_2 = 0;
	bool display2_2 = 0;
	bool display3_2 = 0;
	int display_tablette = 0;
	modes Mode = Ship_Positionning1;
	modes Temp = No_Mode;
	int ShipPositioning = 0;
	glm::vec2 cursor = glm::vec2(1.0,1.0);

	double attack_time = glfwGetTime();
	double key_time = glfwGetTime();

	bool Explosive_flag = true;
	double lastFrameTime = 0.0; // For main loop deltaTime

	while (!glfwWindowShouldClose(window)) {
		double currentFrameTime = glfwGetTime();
		float deltaTime = (float)(currentFrameTime - lastFrameTime);
		lastFrameTime = currentFrameTime;

		cam1.moveCameraIfArrow(window);
		glfwPollEvents();

		UpdateParticles(deltaTime, particles); 

		// Ships positioning
		if (Temp == Explosion) {
			if (Explosive_flag){
				if (Mode == Attack1) {
					cursor = Tablette_2.get_cursor();
					TriggerExplosion(glm::vec3(10-1.0*cursor.y+0.5, 0.0, -10+1.0*cursor.x-0.5), particles);
				}else if (Mode == Attack2) {
					cursor = Tablette_1.get_cursor();
					TriggerExplosion(glm::vec3(25-10+1.0*cursor.y-0.5, 0.0, 8-1.0*cursor.x+0.5), particles);
				}
				Explosive_flag = false;
			}
			if (CheckIfEmpty(particles)) {
				Temp = No_Mode;
				Explosive_flag = true;
				if (Mode == Attack2) {
					display_tablette = 2;
					Tablette_1.set_cursor(1.0, 1.0);
				}
				else if(Mode == Attack1) {
					display_tablette = 1;
					Tablette_2.set_cursor(1.0, 1.0);
				}
			}
		}
		else if (Mode == Ship_Positionning1) {
			switch (ShipPositioning) {
			case 0:
				display1 = true;
				Ship2_1.Positioning(window, currentFrameTime);
				break;
			case 1:
				display2 = true;
				Ship3_1.Positioning(window, currentFrameTime);
				break;
			case 2:
				display3 = true;
				Ship4_1.Positioning(window, currentFrameTime);
				break;
			}
			if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS && (currentFrameTime - key_time) > 0.1) {
				bool change = true;
				switch (ShipPositioning) {
				case 0: {
					glm::mat4 M = Ship2_1.get_model();
					bool orientation = Ship2_1.get_orientation();
					glm::vec2 size = Ship2_1.get_size();
					int Size = size.x * size.y;
					if (!orientation) {
						for (int i = 0; i < Size; i++) {
							if (Base_1.Ships[int(M[3][2] + 9 - i)][9 - int(M[3][0])]) {
								change = false;
							}
						}
						if (change) {
							for (int i = 0; i < Size; i++) {
								Base_1.Ships[int(M[3][2] + 9 - i)][9 - int(M[3][0])] = true;
							}
							Ship2_1.set_model(glm::translate(Ship2_1.get_model(), glm::vec3(0.0, -1.5, 0.0)));
						}


					}
					else {
						for (int i = 0; i < Size; i++) {
							if (Base_1.Ships[int(M[3][2] + 10)][9 - int(M[3][0]) - i]) {
								change = false;
							}
						}
						if (change) {
							for (int i = 0; i < Size; i++) {
								Base_1.Ships[int(M[3][2] + 10)][9 - int(M[3][0]) - i] = true;
							}
							Ship2_1.set_model(glm::translate(Ship2_1.get_model(), glm::vec3(0.0, -1.5, 0.0)));
						}
					}
					break; }
				case 1: {
					glm::mat4 M = Ship3_1.get_model();
					bool orientation = Ship3_1.get_orientation();
					glm::vec2 size = Ship3_1.get_size();
					int Size = size.x * size.y;
					if (!orientation) {
						for (int i = 0; i < Size; i++) {
							if (Base_1.Ships[int(M[3][2] + 9 - i)][9 - int(M[3][0])]) {
								change = false;
							}
						}
						if (change) {
							for (int i = 0; i < Size; i++) {
								Base_1.Ships[int(M[3][2] + 9 - i)][9 - int(M[3][0])] = true;
							}
							Ship3_1.set_model(glm::translate(Ship3_1.get_model(), glm::vec3(0.0, -1.5, 0.0)));
						}
					}
					else {
						for (int i = 0; i < Size; i++) {
							if (Base_1.Ships[int(M[3][2] + 10)][9 - int(M[3][0]) - i]) {
								change = false;
							}
						}
						if (change) {
							for (int i = 0; i < Size; i++) {
								Base_1.Ships[int(M[3][2] + 10)][9 - int(M[3][0]) - i] = true;
							}
							Ship3_1.set_model(glm::translate(Ship3_1.get_model(), glm::vec3(0.0, -1.5, 0.0)));
						}
					}
					break; }
				case 2: {

					glm::mat4 M = Ship4_1.get_model();
					bool orientation = Ship4_1.get_orientation();
					glm::vec2 size = Ship4_1.get_size();
					int Size = size.x * size.y;
					if (!orientation) {
						for (int i = 0; i < Size; i++) {
							if (Base_1.Ships[int(M[3][2] + 9 - i)][9 - int(M[3][0])]) {
								change = false;
							}
						}
						if (change) {
							for (int i = 0; i < Size; i++) {
								Base_1.Ships[int(M[3][2] + 9 - i)][9 - int(M[3][0])] = true;
							}
							Ship4_1.set_model(glm::translate(Ship4_1.get_model(), glm::vec3(0.0, -1.5, 0.0)));
						}
					}
					else {
						for (int i = 0; i < Size; i++) {
							if (Base_1.Ships[int(M[3][2] + 10)][9 - int(M[3][0]) - i]) {
								change = false;
							}
						}
						if (change) {
							for (int i = 0; i < Size; i++) {
								Base_1.Ships[int(M[3][2] + 10)][9 - int(M[3][0]) - i] = true;
							}
							Ship4_1.set_model(glm::translate(Ship4_1.get_model(), glm::vec3(0.0, -1.5, 0.0)));
						}
					}
					break; }
				}
				if (change) {
					ShipPositioning++;
				}
				if (ShipPositioning > 2) {
					ShipPositioning = 0;
					Mode = Ship_Positionning2;
				}
				key_time = glfwGetTime();
			}
		}
		else if (Mode == Ship_Positionning2) {
			switch (ShipPositioning) {
			case 0:
				display1_2 = true;
				Ship2_2.Positioning(window, currentFrameTime);
				break;
			case 1:
				display2_2 = true;
				Ship3_2.Positioning(window, currentFrameTime);
				break;
			case 2:
				display3_2 = true;
				Ship4_2.Positioning(window, currentFrameTime);
				break;
			}
			if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS && (currentFrameTime - key_time) > 0.1) {
				bool change = true;
				switch (ShipPositioning) {
				case 0: {
					glm::mat4 M = Ship2_2.get_model();
					bool orientation = Ship2_2.get_orientation();
					glm::vec2 size = Ship2_2.get_size();
					int Size = size.x * size.y;
					if (!orientation) {
						for (int i = 0; i < Size; i++) {
							if (Base_2.Ships[int(M[3][2] + 9 - i)][9 - int(M[3][0])]) {
								change = false;
							}
						}
						if (change) {
							for (int i = 0; i < Size; i++) {
								Base_2.Ships[int(M[3][2] + 9 - i)][9 - int(M[3][0])] = true;
							}
							Ship2_2.set_model(glm::translate(Ship2_2.get_model(), glm::vec3(0.0, -1.5, 0.0)));
						}
					}
					else {
						for (int i = 0; i < Size; i++) {
							if (Base_2.Ships[int(M[3][2] + 10)][9 - int(M[3][0]) - i]) {
								change = false;
							}
						}
						if (change) {
							for (int i = 0; i < Size; i++) {
								Base_2.Ships[int(M[3][2] + 10)][9 - int(M[3][0]) - i] = true;
							}
							Ship2_2.set_model(glm::translate(Ship2_2.get_model(), glm::vec3(0.0, -1.5, 0.0)));
						}
					}
					break; }
				case 1: {
					glm::mat4 M = Ship3_2.get_model();
					bool orientation = Ship3_2.get_orientation();
					glm::vec2 size = Ship3_2.get_size();
					int Size = size.x * size.y;
					if (!orientation) {
						for (int i = 0; i < Size; i++) {
							if (Base_2.Ships[int(M[3][2] + 9 - i)][9 - int(M[3][0])]) {
								change = false;
							}
						}
						if (change) {
							for (int i = 0; i < Size; i++) {
								Base_2.Ships[int(M[3][2] + 9 - i)][9 - int(M[3][0])] = true;
							}
							Ship3_2.set_model(glm::translate(Ship3_2.get_model(), glm::vec3(0.0, -1.5, 0.0)));
						}
					}
					else {
						for (int i = 0; i < Size; i++) {
							if (Base_2.Ships[int(M[3][2] + 10)][9 - int(M[3][0]) - i]) {
								change = false;
							}
						}
						if (change) {
							for (int i = 0; i < Size; i++) {
								Base_2.Ships[int(M[3][2] + 10)][9 - int(M[3][0]) - i] = true;
							}
							Ship3_2.set_model(glm::translate(Ship3_2.get_model(), glm::vec3(0.0, -1.5, 0.0)));
						}
					}
					break; }
				case 2: {
					glm::mat4 M = Ship4_2.get_model();
					bool orientation = Ship4_2.get_orientation();
					glm::vec2 size = Ship4_2.get_size();
					int Size = size.x * size.y;
					if (!orientation) {
						for (int i = 0; i < Size; i++) {
							if (Base_2.Ships[int(M[3][2] + 9 - i)][9 - int(M[3][0])]) {
								change = false;
							}
						}
						if (change) {
							for (int i = 0; i < Size; i++) {
								Base_2.Ships[int(M[3][2] + 9 - i)][9 - int(M[3][0])] = true;
							}
							Ship4_2.set_model(glm::translate(Ship4_2.get_model(), glm::vec3(0.0, -1.5, 0.0)));
						}
					}
					else {
						for (int i = 0; i < Size; i++) {
							if (Base_2.Ships[int(M[3][2] + 10)][9 - int(M[3][0]) - i]) {
								change = false;
							}
						}
						if (change) {
							for (int i = 0; i < Size; i++) {
								Base_2.Ships[int(M[3][2] + 10)][9 - int(M[3][0]) - i] = true;
							}
							Ship4_2.set_model(glm::translate(Ship4_2.get_model(), glm::vec3(0.0, -1.5, 0.0)));
						}
					}
					break; }
				}
				if (change) {
					ShipPositioning++;
				}
				if (ShipPositioning > 2) {
					Mode = Attack1;
					Tablette_1.set_screen_data(1, 1, choose); // setup for attacking
					display_tablette = 1;

				}
				key_time = glfwGetTime();
			}
		}
		else if (Mode == Attack1) {
			Tablette_1.Choose_Attack(window, currentFrameTime);
			if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS && (currentFrameTime - key_time) > 0.1 && Tablette_1.get_actual_screen_data() == no_state) {
				Mode = Attack2;
				if (Tablette_1.Attack(Base_2)) {
					Temp = Explosion;
					if (Tablette_1.check_win()) {
						Mode = Win1;
					}
				}
				else {
					display_tablette = 2;
					Tablette_1.set_cursor(1.0, 1.0);
				}
				Tablette_2.setup_pre_cursor();
				Tablette_2.set_screen_data(1, 1, choose); // setup for attacking

				key_time = glfwGetTime();
			}
		}
		else if (Mode == Attack2) {
			Tablette_2.Choose_Attack(window, currentFrameTime);
			if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS && (currentFrameTime - key_time) > 0.1 && Tablette_2.get_actual_screen_data() == no_state) {

				Mode = Attack1;
				if (Tablette_2.Attack(Base_1)) {
					Temp = Explosion;
					if (Tablette_2.check_win()) {
						Mode = Win2;
					}
				}
				else {
					display_tablette = 1;
					Tablette_2.set_cursor(1.0, 1.0);
				}
				Tablette_1.setup_pre_cursor();
				Tablette_1.set_screen_data(1, 1, choose); // setup for attacking

				
				key_time = glfwGetTime();
			}
		}
		else if (Mode == Win1) {
		display_tablette = 0;
			std::cout << "First player wins" << std::endl;
		}
		else if (Mode == Win2) {
		display_tablette = 0;
			std::cout << "Second player wins" << std::endl;
		}

		// Graphics card + display
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(program3);

		// BasePlate

		SetObject(Base_1, cam1, u_model3, u_view3, u_perspective3, u_tex3);
		SetObject2(Base_2, cam1, u_model3, u_view3, u_perspective3, u_tex3);

		glUseProgram(program);
		// Ships
		if (display1) {
			SetObject(Ship2_1, cam1, u_model, u_view, u_perspective, u_tex, u_invmodel, u_u_view_pos);
		}
		if (display2) {
			SetObject(Ship3_1, cam1, u_model, u_view, u_perspective, u_tex, u_invmodel, u_u_view_pos);
		}
		if (display3) {
			SetObject(Ship4_1, cam1, u_model, u_view, u_perspective, u_tex, u_invmodel, u_u_view_pos);
		}

		if (display1_2) {
			SetObject2(Ship2_2, cam1, u_model, u_view, u_perspective, u_tex, u_invmodel, u_u_view_pos);
		}
		if (display2_2) {
			SetObject2(Ship3_2, cam1, u_model, u_view, u_perspective, u_tex, u_invmodel, u_u_view_pos);
		}
		if (display3_2) {
			SetObject2(Ship4_2, cam1, u_model, u_view, u_perspective, u_tex, u_invmodel, u_u_view_pos);
		}

		// Tablette
		glUseProgram(program2);
		if (display_tablette == 1) {
			SetObject(Tablette_1, cam1, u_model2, u_tex2, u_cross_tex, u_miss_tex, u_check_tex, u_choose_tex, u_interact);
		}
		else if (display_tablette == 2) {
			SetObject(Tablette_2, cam1, u_model2, u_tex2, u_cross_tex, u_miss_tex, u_check_tex, u_choose_tex, u_interact);
		}

		glDepthFunc(GL_LEQUAL);
		// cubemap
		glUseProgram(program4);
		glUniformMatrix4fv(u_view4, 1, GL_FALSE, glm::value_ptr(cam1.get_view()));
		glUniformMatrix4fv(u_perspective4, 1, GL_FALSE, glm::value_ptr(cam1.get_perspective()));
		glUniform1i(u_tex4, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);

		glBindVertexArray(cubemap.get_VAO());
		glDrawArrays(GL_TRIANGLES, 0, cubemap.get_N());

		glDepthFunc(GL_LESS);


		//explosion
		glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive blending for explosion glow
		glUseProgram(program5);
		glUniformMatrix4fv(u_perspective5, 1, GL_FALSE, glm::value_ptr(cam1.get_perspective()));
		glUniformMatrix4fv(u_view5, 1, GL_FALSE, glm::value_ptr(cam1.get_view()));

		glBindVertexArray(particle.get_VAO());
		glm::mat4 particleModel = glm::mat4(1.0);
		for (const Particle& p : particles) {
			if (p.Life > 0.0f) {
				glUniform3f(u_offset5, p.Position.x, p.Position.y, p.Position.z);
				glUniform4f(u_color5, p.Color.x, p.Color.y, p.Color.z, p.Color.w);
				glUniform1f(u_size5, 0.2f); // You might need to adjust this size

				glUniform1i(u_tex5, 0);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, particle.get_texture());
				glDrawArrays(GL_TRIANGLES, 0, 6); //particle.obj is a quad
			}
		}
		glBindVertexArray(0);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Reset to default blending

		//mirror

		glUseProgram(program6);
		glBindVertexArray(Mirror.get_VAO());
		glUniformMatrix4fv(u_model6, 1, GL_FALSE, glm::value_ptr(Mirror.get_model()));
		glm::mat4 inverseModel = glm::transpose(glm::inverse(Mirror.get_model()));
		glUniformMatrix4fv(u_invmodel6, 1, GL_FALSE, glm::value_ptr(inverseModel));
		glUniformMatrix4fv(u_view6, 1, GL_FALSE, glm::value_ptr(cam1.get_view()));
		glUniformMatrix4fv(u_perspective6, 1, GL_FALSE, glm::value_ptr(cam1.get_perspective()));
		glm::vec3 Camera_pos = cam1.get_position();
		glUniform3f(u_u_view_pos6, Camera_pos.x, Camera_pos.y, Camera_pos.z);
		glUniform1i(u_tex6, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);

		glDrawArrays(GL_TRIANGLES, 0, Mirror.get_N());

		fps(currentFrameTime, cam1);
		glfwSwapBuffers(window);
}


	

	//clean up ressource
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;

}

// Framebuffer size callback function
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);  // Set the new viewport size
	camera* cam = static_cast<camera*>(glfwGetWindowUserPointer(window));
	if (cam) {
		float aspect = (float)width / (float)height;
		cam->setAspectRatio(aspect);
	}
}

GLuint compileShader(std::string shaderCode, GLenum shaderType)
{
	GLuint shader = glCreateShader(shaderType);
	const char* code = shaderCode.c_str();
	glShaderSource(shader, 1, &code, NULL);
	glCompileShader(shader);

	GLchar infoLog[1024];
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 1024, NULL, infoLog);
		std::string t = "undetermined";
		if (shaderType == GL_VERTEX_SHADER) {
			t = "vertex shader";
		}
		else if (shaderType == GL_FRAGMENT_SHADER) {
			t = "fragment shader";
		}
		std::cout << "ERROR::SHADER_COMPILATION_ERROR of the " << t << ": " << shaderType << infoLog << std::endl;
	}
	return shader;
}

GLuint compileProgram(GLuint vertexShader, GLuint fragmentShader)
{
	GLuint programID = glCreateProgram();

	glAttachShader(programID, vertexShader);
	glAttachShader(programID, fragmentShader);
	glLinkProgram(programID);


	GLchar infoLog[1024];
	GLint success;
	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(programID, 1024, NULL, infoLog);
		std::cout << "ERROR::PROGRAM_LINKING_ERROR:  " << infoLog << std::endl;
	}
	return programID;
}

void SetObject(object obj, camera cam, int u_model, int u_view, int u_perspective, int u_tex) {

	SetObject_bis(obj, cam, u_model, u_view, u_perspective, u_tex, obj.get_model());

}

void SetObject(ship obj, camera cam, int u_model, int u_view, int u_perspective, int u_tex, int u_invmodel, int u_u_view_pos) {

	SetObject_bis(obj, cam, u_model, u_view, u_perspective, u_tex, obj.get_model(), u_invmodel, u_u_view_pos);

}

void SetObject(tablette obj, camera cam, int u_model, int u_tex, int u_cross_tex, int u_miss_tex, int u_check_tex, int u_choose_tex, int u_interact) {

	SetObject_bis(obj, cam, u_model, u_tex, obj.get_model(), u_cross_tex, u_miss_tex, u_check_tex, u_choose_tex, u_interact);
	
}

void SetObject2(object obj, camera cam, int u_model, int u_view, int u_perspective, int u_tex) {

	glm::mat4 M = SetObject2_Rotate(obj);
	SetObject_bis(obj,cam,u_model,u_view,u_perspective,u_tex,M);
	
}

void SetObject2(ship obj, camera cam, int u_model, int u_view, int u_perspective, int u_tex, int u_invmodel, int u_u_view_pos) {

	glm::mat4 M = SetObject2_Rotate(obj);
	if (obj.get_orientation()) {
		M = glm::rotate(M, -Pi / 2.0f, glm::vec3(0.0, 1.0, 0.0));
	}
	SetObject_bis(obj, cam, u_model, u_view, u_perspective, u_tex, M, u_invmodel, u_u_view_pos);

}

void SetObject2(tablette obj, camera cam, int u_model, int u_tex, int u_cross_tex, int u_miss_tex, int u_check_tex, int u_choose_tex, int u_interact) {

	glm::mat4 M = SetObject2_Rotate(obj);
	SetObject_bis(obj, cam, u_model, u_tex, M, u_cross_tex, u_miss_tex, u_check_tex, u_choose_tex, u_interact);

}

glm::mat4 SetObject2_Rotate(object obj) {
	glm::vec4 NewOrigin = glm::vec4(25.0, 0.0, -2.0, 1.0);
	glm::mat4 M = obj.get_model();
	glm::vec3 NewCoordinates = M[3];
	glm::vec3 scale;

	scale.x = glm::length(glm::vec3(M[0])); // length of first column
	scale.y = glm::length(glm::vec3(M[1])); // second column
	scale.z = glm::length(glm::vec3(M[2])); // third column

	M = glm::mat4(1.0);
	M[3] = NewOrigin;
	M = glm::rotate(M, -Pi, glm::vec3(0.0, 1.0, 0.0));
	M = glm::scale(M, scale);
	M = glm::translate(M, NewCoordinates / scale);
	return M;
}
void SetObject_bis(object obj, camera cam, int u_model, int u_view, int u_perspective, int u_tex, glm::mat4 M) {
	glBindVertexArray(obj.get_VAO());
	glUniformMatrix4fv(u_model, 1, GL_FALSE, glm::value_ptr(M));
	glUniformMatrix4fv(u_view, 1, GL_FALSE, glm::value_ptr(cam.get_view()));
	glUniformMatrix4fv(u_perspective, 1, GL_FALSE, glm::value_ptr(cam.get_perspective()));

	glUniform1i(u_tex, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, obj.get_texture());

	glDrawArrays(GL_TRIANGLES, 0, obj.get_N());
}
void SetObject_bis(ship obj, camera cam, int u_model, int u_view, int u_perspective, int u_tex, glm::mat4 M, int u_invmodel, int u_u_view_pos) {
	glBindVertexArray(obj.get_VAO());
	glUniformMatrix4fv(u_model, 1, GL_FALSE, glm::value_ptr(M));
	glm::mat4 inverseModel = glm::transpose(glm::inverse(M));
	glUniformMatrix4fv(u_invmodel, 1, GL_FALSE, glm::value_ptr(inverseModel));
	glUniformMatrix4fv(u_view, 1, GL_FALSE, glm::value_ptr(cam.get_view()));
	glUniformMatrix4fv(u_perspective, 1, GL_FALSE, glm::value_ptr(cam.get_perspective()));
	glm::vec3 Camera_pos = cam.get_position();
	glUniform3f(u_u_view_pos, Camera_pos.x,Camera_pos.y,Camera_pos.z);

	glUniform1i(u_tex, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, obj.get_texture());

	glDrawArrays(GL_TRIANGLES, 0, obj.get_N());
}
void SetObject_bis(tablette obj, camera cam, int u_model, int u_tex, glm::mat4 M, int u_cross_tex, int u_miss_tex, int u_check_tex, int u_choose_tex, int u_interact) {
	glBindVertexArray(obj.get_VAO());
	glUniformMatrix4fv(u_model, 1, GL_FALSE, glm::value_ptr(M));

	glUniform1i(u_tex, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, obj.get_texture());

	glUniform1i(u_cross_tex, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, obj.get_cross_tex());

	glUniform1i(u_miss_tex, 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, obj.get_miss_tex());

	glUniform1i(u_check_tex, 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, obj.get_check_tex());

	glUniform1i(u_choose_tex, 4);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, obj.get_choose_tex());

	glUniform1i(u_interact, 5);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, obj.get_interact());

	glDrawArrays(GL_TRIANGLES, 0, obj.get_N());
}
void loadCubemapFace(const char* path, const GLenum& targetFace)
{
	int imWidth, imHeight, imNrChannels;
	unsigned char* data = stbi_load(path, &imWidth, &imHeight, &imNrChannels, 0);
	if (data)
	{

		glTexImage2D(targetFace, 0, GL_RGB, imWidth, imHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		//glGenerateMipmap(targetFace);
	}
	else {
		std::cout << "Failed to Load texture" << std::endl;
		const char* reason = stbi_failure_reason();
		std::cout << (reason == NULL ? "Probably not implemented by the student" : reason) << std::endl;
	}
	stbi_image_free(data);
}

void RespawnParticle(Particle& particle, glm::vec3 explosionOrigin) {
	float speed = static_cast<float>((rand() % 100) / 100.0f * 5.0f); // Up to 5 units/sec
	glm::vec3 dir = glm::sphericalRand(1.0f); // From GLM: random unit vector
	particle.Position = explosionOrigin;
	particle.Velocity = dir * speed;
	particle.Color = glm::vec4(1.0f, 0.5f + static_cast<float>((rand() % 50) / 100.0f), 0.0f, 1.0f);
	particle.Life = 1.0f + static_cast<float>((rand() % 100) / 100.0f); // 1–2 seconds
}

void UpdateParticles(float dt, std::vector<Particle>& particles) {
	for (Particle& p : particles) {
		p.Life -= dt;
		if (p.Life > 0.0f) {
			// Gravity effect
			p.Velocity += glm::vec3(0.0f, -9.81f, 0.0f) * dt;
			p.Position += p.Velocity * dt;
			p.Color.a -= dt * 1.5f; // Fade out
		}
	}
}

void TriggerExplosion(glm::vec3 center, std::vector<Particle>& particles) {
	int count = 400; // Particles per explosion
	for (int i = 0; i < count; ++i) {
		int idx = FirstUnusedParticle(particles);
		RespawnParticle(particles[idx], center);
	}
}

unsigned int FirstUnusedParticle(std::vector<Particle> particles) {
	for (unsigned int i = 0; i < particles.size(); ++i) {
		if (particles[i].Life <= 0.0f)
			return i;
	}
	return 0; // fallback
}

bool CheckIfEmpty(std::vector<Particle> particles) {
	for (unsigned int i = 0; i < particles.size(); ++i) {
		if (particles[i].Life >= 0.0f)
			return false;
	}
	return true;
}