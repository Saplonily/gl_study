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
private:
	LPVOID m_res_ptr;
	HGLOBAL m_res_handle;
	DWORD m_len;

public:
	win_res(const win_res& other) = delete;
	win_res(win_res&& other) = delete;
	win_res(LPCSTR resName, LPCSTR resType, HMODULE module = nullptr)
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
		m_res_ptr = ptr;
		m_res_handle = res;
		m_len = len;
	}

	LPVOID res_ptr() { return m_res_ptr; }
	template<class TPtr> TPtr* res_ptr() { return (TPtr*)m_res_ptr; }
	HGLOBAL res_handle() { return m_res_handle; }
	DWORD res_len() { return m_len; }

	~win_res()
	{
		FreeResource(m_res_handle);
	}
};
}
#endif