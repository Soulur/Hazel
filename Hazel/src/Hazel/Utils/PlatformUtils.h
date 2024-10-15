#pragma once

#include <string>
#include <optional>

namespace Hazel {

	class FileDialogs
	{
	public:
		// These return empty strings if cancelled
		static std::optional<std::string> OpenFile(const char* filer);
		static std::optional<std::string> SaveFile(const char* filer);
	};

}