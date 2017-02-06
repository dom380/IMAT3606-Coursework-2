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