#pragma once

#include <glm/glm.hpp>

#include "SceneCamera.h"
#include "ScriptableEntity.h"

namespace Hazel {

	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}
	};

	struct TransformComponent
	{
		glm::mat4 Transform = glm::mat4(1.0f);

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::mat4& transform)
			: Transform(transform) {}

		operator glm::mat4& () { return Transform; }
		operator const glm::mat4& () const { return Transform; }
	};

	struct SpriteRendererComponent
	{
		glm::vec4 Color { 1.0f ,1.0f ,1.0f ,1.0f };

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color)
			: Color(color) {}
	};

	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = true; // TODO: think about moving to Scene
		bool FixedAspectRatio = false;
		
		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		std::function<void()> InstantiateFunciton;
		std::function<void()> DestroyInstanceFunciton;

		std::function<void(ScriptableEntity*)> OnCreateFunciton;
		std::function<void(ScriptableEntity*)> OnDestroyFunciton;
		std::function<void(ScriptableEntity* , Timestep)> OnUpdateFunciton;

		template<typename T>
		void Bind()
		{
			InstantiateFunciton		= [&]() { Instance = new T(); };
			DestroyInstanceFunciton = [&]() { delete (T*)Instance; Instance = nullptr; };

			OnCreateFunciton	= [](ScriptableEntity* instance) { ((T*)instance)->OnCreate(); };
			OnDestroyFunciton	= [](ScriptableEntity* instance) { ((T*)instance)->OnDestroy(); };
			OnUpdateFunciton	= [](ScriptableEntity* instance , Timestep ts) { ((T*)instance)->OnUpdate(ts); };
		}
	};

}