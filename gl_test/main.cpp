#include "pch.h"

namespace ggbs {

bool do_one_frame(GLFWwindow* win);
void prepare(GLFWwindow* win);
GLFWwindow* init();
void shutdown(GLFWwindow* win);

}

int main()
{
	using namespace ggbs;
	auto win = init();
	prepare(win);
	while (do_one_frame(win)) {}
	shutdown(win);
}