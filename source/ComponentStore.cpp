#include <ComponentStore.h>

Handle ComponentStore::storeComponent(std::shared_ptr<ModelComponent> component)
{
	return models.add(*component);
}

Handle ComponentStore::storeComponent(std::shared_ptr<LogicComponent> component)
{
	return logic.add(*component);
}

Handle ComponentStore::storeComponent(std::shared_ptr<Transform> component)
{
	return transforms.add(*component);
}

Handle ComponentStore::storeComponent(std::shared_ptr<PhysicsComponent> component)
{
	return physics.add(*component);
}

Handle ComponentStore::storeComponent(std::shared_ptr<CollisionTrigger> component)
{
	return triggers.add(*component);
}

Handle ComponentStore::storeComponent(std::shared_ptr<ControllerComponent> component)
{
	return controllers.add(*component);
}

Handle ComponentStore::storeComponent(std::shared_ptr<AnimatedModelComponent> component)
{
	return animatedModels.add(*component);
}

void ComponentStore::dispose()
{
	auto modelComps = models.getAll();
	for (auto it = modelComps->begin(); it != modelComps->end(); ++it)
	{
		if (it->first != -1)
		{
			it->second.dispose();
		}
	}
	auto logicComps = logic.getAll();
	for (auto it = logicComps->begin(); it != logicComps->end(); ++it)
	{
		if (it->first != -1)
		{
			it->second.dispose();
		}
	}
	auto transformComps = transforms.getAll();
	for (auto it = transformComps->begin(); it != transformComps->end(); ++it)
	{
		if (it->first != -1)
		{
			it->second.dispose();
		}
	}
	auto controllerComps = controllers.getAll();
	for (auto it = controllerComps->begin(); it != controllerComps->end(); ++it)
	{
		if (it->first != -1)
		{
			it->second.dispose();
		}
	}
	auto physicsComps = physics.getAll();
	for (auto it = physicsComps->begin(); it != physicsComps->end(); ++it)
	{
		if (it->first != -1)
		{
			it->second.dispose();
		}
	}
	auto triggerComps = triggers.getAll();
	for (auto it = triggerComps->begin(); it != triggerComps->end(); ++it)
	{
		if (it->first != -1)
		{
			it->second.dispose();
		}
	}
}
