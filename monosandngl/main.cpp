#include "pch.h"

struct out_proxy;

template<class T>
static out_proxy outl(const T& thing);
template<class T>
static out_proxy out(const T& thing);

struct out_proxy
{
	template<class T> out_proxy outl(const T& thing) { return ::outl(thing); }
	template<class T> out_proxy out(const T& thing) { return ::out(thing); }
};

template<class T>
static out_proxy outl(const T& thing)
{
	std::cout << thing << '\n';
	return out_proxy();
}

template<class T>
static out_proxy out(const T& thing)
{
	std::cout << thing;
	return out_proxy();
}




int main()
{
	
}