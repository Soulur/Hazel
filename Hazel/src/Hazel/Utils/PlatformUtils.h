#pragma once

#include <string>

namespace Hazel {

	class FileDialogs
	{
	public:
		// These return empty strings if cancelled
		static std::string OpenFile(const char* filer);
		static std::string SaveFile(const char* filer);
	};

}