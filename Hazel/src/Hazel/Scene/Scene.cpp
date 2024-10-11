#include "hzpch.h"
#include "Scene.h"

#include "Component.h"
#include "Hazel/Renderer/Renderer2D.h"

#include <glm/glm.hpp>

#include "Entity.h"

namespace Hazel {

	static void DoMath(const glm::mat4 transform)
	{

	}

	static void OnTransformConstruct(entt::registry& registry, entt::entity entity)
	{
		
	}

	Scene::Scene()
	{
#if ENTT_EXAMPE_CODE



		entt::entity entity = m_Registry.create();
		m_Registry.emplace<TransformComponent>(entity, glm::mat4(1.0f));

		m_Registry.on_construct<TransformComponent>().connect<&OnTransformConstruct>();

		TransformComponent& transform = m_Registry.get<TransformComponent>(entity);

		auto view = m_Registry.view<TransformComponent>();
		for (auto entity : view)
		{
			TransformComponent& transform = m_Registry.get<TransformComponent>(entity);
		}

		auto group = m_Registry.group<TransformComponent>(entt::get<MeshComponent>);
		for (auto entity : group)
		{
			auto&[transform,mesh] = group.get<TransformComponent, MeshComponent>(entity);
		}
#endif // ENTT_EXAMPE_CODE
	}

	Scene::~Scene()
	{
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create() , this };
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		return entity;
	}

	void Scene::OnUpdate(Timestep ts)
	{
		// Update scripts
		{
			m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
			{
				if (!nsc.Instance)
				{
					nsc.InstantiateFunciton();
					nsc.Instance->m_Entity = { entity , this };
					nsc.OnCreateFunciton(nsc.Instance);
				}

				nsc.OnUpdateFunciton(nsc.Instance, ts);
			});
		}
		

		Camera* mainCamera = nullptr;
		glm::mat4* mainTransform = nullptr;
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto& [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

				if (camera.Primary)
				{
					mainCamera = &camera.Camera;
					mainTransform = &transform.Transform;
					break;
				}
			}
		}

		if (mainCamera)
		{
			Renderer2D::BeginScene(mainCamera->GetProjection(), *mainTransform);

			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group)
			{
				auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

				Renderer2D::DrawQuad(transform, sprite.Color);
			}

			Renderer2D::EndScene();
		}
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		// Resize our non-FixedAspectRatio camera;
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio)
				cameraComponent.Camera.SetViewportSize(width, height);
		}
	}

}