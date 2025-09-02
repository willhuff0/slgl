#include "WebGPUContext.hpp"

namespace SLGL::Graphics {
    WebGPU::Context::~Context() {
        device.release();
    }
}
