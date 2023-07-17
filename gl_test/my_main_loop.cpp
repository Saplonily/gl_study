#include "pch.h"
#include "main_loop.h"
#include "win_res.h"
#include "shader.h"
#include "resource.h"
#include "proj_imgui.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>
#include <thread>
#include <chrono>

namespace ggbs {

extern float vertices[180];

#pragma region callbacks
void ErrCallback(int code, const char* msg)
{
	std::cout << "=== glfw ERROR " << code << " ===" << std::endl;
	std::cout << "=== " << msg << std::endl;

}

void FramebufferSizeCallback(GLFWwindow* win, int width, int height)
{
	glViewport(0, 0, width, height);
	main_loop::get_ins()->wWidth = width;
	main_loop::get_ins()->wHeight = height;
}

void CursorPosCallback(GLFWwindow* win, double x, double y)
{
	main_loop::get_ins()->mx = x;
	main_loop::get_ins()->my = y;
}
#pragma endregion

main_loop* main_loop::instance = nullptr;

main_loop* main_loop::get_ins() { return main_loop::instance; }

void main_loop::init()
{
	SetConsoleOutputCP(CP_UTF8);
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_SAMPLES, 16);
	glfwSetErrorCallback(ErrCallback);

	GLFWwindow* win = glfwCreateWindow(wWidth = 1200, wHeight = 800, "你好这里是窗口标题...!", nullptr, nullptr);
	glfwMakeContextCurrent(win);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSetFramebufferSizeCallback(win, FramebufferSizeCallback);
	glfwSetCursorPosCallback(win, CursorPosCallback);
	this->win = win;
}

void main_loop::prepare()
{
#pragma region like_opengl_prepare
	HWND handle = glfwGetWin32Window(win);
	HMENU menu = GetSystemMenu(handle, false);
	AppendMenuW(menu, MF_STRING, NULL, L"第一个窗口独有~.");

	auto fsh = win_res::find_and_lock(MAKEINTRESOURCEA(IDR_FSH_SHD1), "SHD");
	auto vsh = win_res::find_and_lock(MAKEINTRESOURCEA(IDR_VSH_SHD1), "SHD");
	shader = std::make_unique<Shader>(static_cast<char*>(std::get<0>(vsh)), static_cast<char*>(std::get<0>(fsh)));
	win_res::free(fsh);
	win_res::free(vsh);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	// 来点 vao
	glGenVertexArrays(1, &vaoId);
	// 来点 vbo 和 ebo
	glGenBuffers(1, &vboId);

	// 绑定绑定还有数据!
	glBindVertexArray(vaoId);

	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// shader 需要这些顶点数据!
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0); // 位置
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1); // 纹理坐标

	// 加载贴图
	stbi_set_flip_vertically_on_load(true);
	int channels;
	auto tuple = win_res::find_and_lock(MAKEINTRESOURCEA(IDB_PNG1), "PNG");
	byte* img = stbi_load_from_memory(static_cast<stbi_uc*>(std::get<0>(tuple)), std::get<2>(tuple), &texWidth, &texHeight, &channels, 0);

	int align = -1;
	int total = texWidth * texHeight;
	if (total % 8 == 0)
		align = 8;
	else if (total % 4 == 0)
		align = 4;
	else if (total % 2 == 0)
		align = 2;
	else
		align = 1;
	glPixelStorei(GL_UNPACK_ALIGNMENT, align);
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	glGenerateMipmap(GL_TEXTURE_2D);
	win_res::free(tuple);
#pragma endregion

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	const ImGuiIO io = ImGui::GetIO();

	ImGui_ImplGlfw_InitForOpenGL(win, true);
	ImGui_ImplOpenGL3_Init();
}

main_loop::main_loop()
{
	assert(!instance);
	instance = this;
}

void main_loop::run()
{
	namespace sc = std::chrono;
	using namespace std::chrono_literals;
	constexpr sc::duration<float> expected_delta = 1s / 60.0f;

	init();
	prepare();
	float pre_delta = 1 / 60.0f;
	while (!requestedExit)
	{
		sc::steady_clock::time_point before = std::chrono::steady_clock::now();
		update(pre_delta);
		draw();
		sc::duration<float> passed = std::chrono::steady_clock::now() - before;
		sc::duration<float> to_sleep = expected_delta - passed;
		pre_delta = fmaxf(passed.count(), expected_delta.count());
		passed_frames++;
		std::cout << "exp: " << expected_delta.count() << " act: " << passed.count() << std::endl;
		if (to_sleep >= 0ms)
			std::this_thread::sleep_for(to_sleep);
	}
	shutdown();
}

static std::string inputStr;
static float f = 1.0f;
void main_loop::update(float delta)
{
	if (glfwWindowShouldClose(win))
		requestedExit = true;
	glfwPollEvents();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Spacing();
	glm::vec4 myVec(0.0f, 1.0f, 0.0f, 1.0f);
	ImGui::TextColored(reinterpret_cast<ImVec4&>(myVec), "Hello I'm colored!");
	ImGui::InputText("string", &inputStr);
	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
	ImGui::NewLine();
	if (ImGui::Button("Fuck me"))
	{
		std::cout << "fuck content: " << inputStr << std::endl;
	}
}

void main_loop::draw()
{
	glClearColor(0.1f, 0.2f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	shader->use();
	//绑定纹理
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texId);
	shader->setInt("texture1", 0);

	glm::mat4 model{1.0f};
	glm::mat4 view{1.0f};
	glm::mat4 projection{1.0f};

	float v = static_cast<float>(glfwGetTime());
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.0f));
	//model = glm::rotate(model, 0.0f, glm::vec3(-1.2f, 1.0f, 1.5f));

	view = glm::lookAt(glm::vec3(0.0f, 3.0f, 5.0f), glm::vec3(sinf(v), 2.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	//projection = glm::ortho(-6.0f, 6.0f, -3.0f, 3.0f, 0.01f, 1000.0f);
	projection = glm::perspective(glm::radians(45.0f), (float)wWidth / wHeight, 0.1f, 1000.0f);

	shader->setMat4("model", model);
	shader->setMat4("view", view);
	shader->setMat4("projection", projection);

	glBindVertexArray(vaoId);

	for (int i = -5; i < 5; i++)
		for (int j = static_cast<int>(-5 * 10 * f); j < 5; j++)
		{
			glm::vec3 pos(i, 0.0f, j);
			model = glm::translate(glm::mat4(1.0f), pos);
			shader->setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 180 / (3 + 2));
		}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(win);
}

void main_loop::shutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
}

}