#include "PluginManager.h"
#include <iostream>
#ifdef _WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

PluginManager::~PluginManager() {
    unloadAll();
}

bool PluginManager::loadPlugin(const LAACHAR_T* path) {
    assert(path);
    if (!path)
        return false;
#ifdef _WIN32
    HMODULE h = LoadLibraryW(path);
    if (!h) {
        std::cerr << "LoadLibrary failed: " << path << std::endl;
        return false;
    }
    auto create = (CreatePluginFn)GetProcAddress(h, "CreatePlugin");
    auto destroy = (DestroyPluginFn)GetProcAddress(h, "DestroyPlugin");
#else
    void* h = dlopen(path, RTLD_NOW);
    if (!h) {
        std::cerr << "dlopen failed: " << dlerror() << std::endl;
        return false;
    }
    auto create = (CreatePluginFn)dlsym(h, "CreatePlugin");
    auto destroy = (DestroyPluginFn)dlsym(h, "DestroyPlugin");
#endif
    if (!create || !destroy) {
        std::cerr << "Plugin factory functions not found: " << path << std::endl;
#ifdef _WIN32
        FreeLibrary(h);
#else
        dlclose(h);
#endif
        return false;
    }

    IPlugin* plugin = create();
    if (!plugin) {
        std::cerr << "CreatePlugin returned null: " << path << std::endl;
#ifdef _WIN32
        FreeLibrary(h);
#else
        dlclose(h);
#endif
        return false;
    }

    PluginRecord rec;
    rec.handle = h;
    rec.instance = plugin;
    rec.destroyFn = destroy;

    size_t idx = plugins_.size();
    plugins_.push_back(rec);

    // register capabilities
    try {
        /*auto caps = plugin->capabilities();
        for (auto &c : caps) {
            std::string name = c["name"].get<std::string>();
            capabilityMap_.emplace(name, idx);
        }*/
    } catch (...) {
        // ignore
    }

    std::cout << "Loaded plugin: " << plugin->name() << std::endl;
    return true;
}

void PluginManager::unloadAll() {
    for (auto &pr : plugins_) {
        if (pr.instance && pr.destroyFn) {
            pr.destroyFn(pr.instance);
            pr.instance = nullptr;
        }
        if (pr.handle) {
#ifdef _WIN32
            FreeLibrary((HMODULE)pr.handle);
#else
            dlclose(pr.handle);
#endif
            pr.handle = nullptr;
        }
    }
    plugins_.clear();
    capabilityMap_.clear();
}

nlohmann::json PluginManager::executeCapability(const std::string& capabilityName, const nlohmann::json& args) {
    auto it = capabilityMap_.find(capabilityName);
    if (it == capabilityMap_.end()) {
        return { {"ok", false}, {"error", "capability not found"} };
    }
    size_t idx = it->second;
    auto pr = plugins_[idx].instance;
    if (!pr) return { {"ok", false}, {"error", "plugin instance null"} };

    try {
        //return pr->execute(capabilityName, args);
		return { {"ok", true}, {"reply", "not implemented"} };
    } catch (const std::exception &e) {
        return { {"ok", false}, {"error", e.what()} };
    }
}

std::vector<std::string> PluginManager::listCapabilities() const {
    std::vector<std::string> out;
    out.reserve(capabilityMap_.size());
    for (auto &kv : capabilityMap_) out.push_back(kv.first);
    return out;
}

std::string PluginManager::execute(const FunctionCall& fnCall) {
    auto resultJson = executeCapability(fnCall.name, nlohmann::json::parse(fnCall.argumentsJSON));
    if (!resultJson.contains("ok") || !resultJson["ok"].get<bool>()) {
        return "Error executing capability: " + (resultJson.contains("error") ? resultJson["error"].get<std::string>() : "unknown error");
    }
    return resultJson.contains("reply") ? resultJson["reply"].get<std::string>() : "";
}