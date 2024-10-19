#include <Hazel.h>
// ---Entry Point---------------------
#include <Hazel/Core/EntryPoint.h>
// -----------------------------------

#include "EditorLayer.h"

namespace Hazel {


	class Hazelnut : public Application
	{
	public:
		Hazelnut()
			: Application("Hazelnut")
		{
			PushLayer(new EditorLayer());
		}

		~Hazelnut()
		{
		}

	};


	Application* CreatApplication()
	{
		return new Hazelnut();
	}
}