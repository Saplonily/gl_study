#pragma once
#ifndef H_MAIN_LOOP
#define H_MAIN_LOOP
#include <GLFW/glfw3.h>
#include "shader.h"
#include <memory>
namespace ggbs {

class main_loop
{
public:
	static main_loop* instance;

	GLFWwindow* win = nullptr;
	std::unique_ptr<Shader> shader = nullptr;
	unsigned int vboId = -1, vaoId = -1, texId = -1;
	int passed_frames = 0;
	int texWidth = -1, texHeight = -1;
	int wWidth = -1, wHeight = -1;
	double mx = -1, my = -1;
	bool requestedExit = false;
public:
	static main_loop* get_ins();
	void init();
	void prepare();
	void update(float delta);
	void draw();
	void shutdown();
	void run();
	main_loop();
};

}
#endif