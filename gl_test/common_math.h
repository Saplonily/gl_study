#pragma once
#ifndef H_COMMON_MATH
#define H_COMMON_MATH

#define _USE_MATH_DEFINES
#include <math.h>
#include <assert.h>

namespace common_math {

inline float wavef(float v, float cycle, float min, float max, float start = 0.0f)
{
	assert(max >= min);
	float h = (max + min) / 2.0f;
	float a = max - h;
	float w = static_cast<float>(M_PI) * 2.0f / cycle;
	float phi = w * start;
	return a * sinf(w * (v - phi)) + h;
}

inline double wave(double v, double cycle, double min, double max, double start = 0.0)
{
	assert(max >= min);
	double h = (max + min) / 2.0f;
	double a = max - h;
	double w = static_cast<double>(M_PI) * 2.0f / cycle;
	double phi = w * start;
	return a * sin(w * (v - phi)) + h;
}

}

#endif