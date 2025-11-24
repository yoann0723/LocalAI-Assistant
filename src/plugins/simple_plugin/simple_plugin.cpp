#include "simple_plugin.h"

class SamplePlugin : public IPlugin {
public:
    const char* name() const override { return "SamplePlugin"; }
    const char* capabilities() const override {
        return R"({ { {"name", "echo"}, {"params", nlohmann::json::object()} } })";
    }
    ExcutionResult execute(const char* capabilityName, const char * args) override {
        if (capabilityName == "echo") {
            return {};
        }
        return {};
    }
};


IPlugin* CreatePlugin() {
    return new SamplePlugin();
}

void DestroyPlugin(IPlugin* p) {
    delete p;
}