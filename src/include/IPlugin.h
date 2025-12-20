#pragma once
#include <string>
#include <json.hpp>

// Minimal plugin interface used by PluginManager.
// Plugins should expose C factory functions (see example plugin).

struct ExcutionResult {
    bool ok;
    std::string reply;
    std::string error;
};

class IPlugin {
public:
    virtual ~IPlugin() = default;
    // plugin id/name
    virtual const char* name() const = 0;
    // register capabilities: returns a json array describing functions
    // Example: [{"name":"open_app","params":{"app_name":"string"}}]
    virtual const char* capabilities() const = 0;

    // Execute a capability with json args and return JSON result
    virtual ExcutionResult execute(const char* capabilityName, const char* args) = 0;
};

// C-style plugin factory signatures that DLLs should export
extern "C" {
    typedef IPlugin* (*CreatePluginFn)();
    typedef void (*DestroyPluginFn)(IPlugin*);
}