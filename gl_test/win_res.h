#pragma once
#include <exception>
#include <Windows.h>
#include <utility>
#include <tuple>

#ifndef H_WIN_RES
#define H_WIN_RES

namespace ggbs {

class bad_resource : std::exception
{
public:
	LPCSTR resName;
	LPCSTR resType;
	bool is_not_found;

	bad_resource(LPCSTR resName, LPCSTR resType, bool is_not_found = true)
		: resName(resName), resType(resType), is_not_found(is_not_found)
	{
	}

	const char* what() const override
	{
		return "Bad resource.";
	}
};

class win_res
{
public:
	win_res() = delete;
	win_res(const win_res& other) = delete;
	win_res(win_res&& other) = delete;

	static std::tuple<LPVOID, HGLOBAL, DWORD> find_and_lock(LPCSTR resName, LPCSTR resType, HMODULE module = nullptr)
	{
		HMODULE md;
		if (module != nullptr)
			md = module;
		else
			md = GetModuleHandleA(nullptr);

		HRSRC hrsrc = FindResourceA(md, resName, resType);
		if (!hrsrc)
		{
			throw bad_resource(resName, resType);
		}

		DWORD len = SizeofResource(md, hrsrc);
		HGLOBAL res = LoadResource(md, hrsrc);
		if (!res)
		{
			throw bad_resource(resName, resType, false);
		}
		LPVOID ptr = LockResource(res);
		return std::make_tuple(ptr, res, len);
	}

	static void free(std::tuple<LPVOID, HGLOBAL, DWORD>& resTuple)
	{
		FreeResource(std::get<1>(resTuple));
		std::get<0>(resTuple) = nullptr;
	}
};



}
#endif