#pragma once


#ifdef HZ_PLATFORM_WINDOWS

extern Hazel::Application* Hazel::CreatApplication();

int main(int argc , char** argv)
{
	Hazel::Log::Init();
	HZ_CORE_WARN("Initalized Log!");
	int a = 5;
	HZ_INFO("Hello! Var={0}" , a);

	auto app = Hazel::CreatApplication();
	app->Run();
	delete app;
}

#else
	#error Hazel only support Window!
#endif // HZ_PLATFORM_WINDOWS
