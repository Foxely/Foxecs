#pragma once

#include <memory>
#include "Core/System.hpp"

class Event;


class RenderSystem : public System
{
public:
	RenderSystem(World& oWorld);

	void Update(float dt);

private:
	Entity mCamera;
};
