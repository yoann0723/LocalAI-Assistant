#pragma once
#ifdef __cplusplus
#include "localai_c_api.h"
#include <stdexcept>
#include <memory>

namespace localai {

/* Exception type for C++ wrapper */
class LocalAIError : public std::runtime_error {
public:
	explicit LocalAIError(const std::string& m) : std::runtime_error(m) {}
};

//struct LocalAI_Status_deleter {
//    void operator()(LocalAI_Status* status) const {
//        if (status) {
//            LocalAI_StatusDestroy(status);
//        }
//	}
//};
//
//using LocalAI_StatusPtr = std::unique_ptr<LocalAI_Status, LocalAI_Status_deleter>;

template<typename T, void release(T)> class PtrAutoRelease {
protected:
	T val;

public:
	inline PtrAutoRelease() : val(nullptr) {}
	inline PtrAutoRelease(T val_) : val(val_) {}
	PtrAutoRelease(const PtrAutoRelease& ref) = delete;
	inline PtrAutoRelease(PtrAutoRelease&& ref) : val(ref.val) { ref.val = nullptr; }

	inline ~PtrAutoRelease() { release(val); }

	inline operator T() const { return val; }
	inline T Get() const { return val; }

	inline bool operator==(T p) const { return val == p; }
	inline bool operator!=(T p) const { return val != p; }

	inline PtrAutoRelease& operator=(PtrAutoRelease&& ref)
	{
		if (this != &ref) {
			release(val);
			val = ref.val;
			ref.val = nullptr;
		}

		return *this;
	}

	inline PtrAutoRelease& operator=(T new_val)
	{
		release(val);
		val = new_val;
		return *this;
	}
};

using LocalAI_StatusPtr = PtrAutoRelease<LocalAI_Status*, LocalAI_StatusDestroy>;

/* RAII LocalAI instance */
class LocalAI {
public:
    LocalAI(LocalAI_Config config) 
	{
		LocalAI_StatusPtr status = LocalAI_Core_Initialize(config);
		if (status) {
			const char* msg = LocalAI_StatusGetMessage(status.Get());
			throw LocalAIError(msg);
		}
	}

	~LocalAI() {
		LocalAI_Core_Shutdown();
	}

	LocalAI_StatusPtr initializeModel(Model_Type type, const char *model_path, const Model_Params &params) 
	{
		return LocalAI_Core_InitializeModel(type, model_path, params);
	}

	/* Disable copy and move */
	LocalAI(const LocalAI&) = delete;
	LocalAI& operator=(const LocalAI&) = delete;
	LocalAI(LocalAI&&) = delete;

	void setLogLevel(int lvl) { LocalAI_Core_SetLogLevel(lvl); }

};

}

#endif
