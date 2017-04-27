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