#include "WebGPUDebug.hpp"

namespace SLGL::Graphics {
    void WebGPU::DebugPrintAdapter(wgpu::Adapter& adapter) {
        AdapterInfo adapterInfo;
        adapter.getInfo(&adapterInfo);
        std::cout << "Adapter: " << wgpuAdapterTypeNames.at(adapterInfo.adapterType) << " "
                  << std::string(adapterInfo.device.data, adapterInfo.device.length) << std::endl;
        std::cout << " Driver: " << wgpuBackendTypeNames.at(adapterInfo.backendType) << " "
                  << std::string(adapterInfo.description.data, adapterInfo.description.length)<< std::endl;
    }

    void WebGPU::DebugPrintPresentMode(WGPUPresentMode presentMode) {
        std::cout << "Present: " << wgpuPresentModeNames.at(presentMode) << std::endl << std::endl;
    }
}