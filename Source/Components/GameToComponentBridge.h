#pragma once

#include "../DXGIWindowWrapper.h"
#include "../DeviceWrapper.h"

namespace dmbrn {
struct GameToComponentBridge {
    DeviceWrapper& device;
    DXGIWindowWrapper& window;
};
}