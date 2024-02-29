#pragma once

#include "../DeviceWrapper.h"
#include "../DXGIWindowWrapper.h"

namespace dmbrn
{
	struct GameToComponentBridge
	{
		DeviceWrapper& device;
		DXGIWindowWrapper& window;
	};
}