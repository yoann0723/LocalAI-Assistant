/**
 * This file is part of LocalAI-Assistant
 * Manages load/unload of AI plugins and their capabilities
 */
#pragma once
#include "IPlugin.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "core_common.h"
#include "LLMEngine.h"

class PluginManager {
public:
    PluginManager() = default;
    ~PluginManager();

    // Load plugin by path (DLL/.so)
    bool loadPlugin(const LAACHAR_T* path);

    // Unload all plugins
    void unloadAll();

    // Find capability and execute
    nlohmann::json executeCapability(const std::string& capabilityName, const nlohmann::json& args);

    // Execute from FunctionCall
    std::string execute(const FunctionCall& fnCall);

    // list capabilities
    std::vector<std::string> listCapabilities() const;
private:
    struct PluginRecord {
        void* handle = nullptr; // OS handle
        IPlugin* instance = nullptr;
        DestroyPluginFn destroyFn = nullptr;
    };

    std::vector<PluginRecord> plugins_;
    // map capability -> plugin index
    std::unordered_map<std::string, size_t> capabilityMap_;
};