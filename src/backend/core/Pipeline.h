#pragma once
#include "ILLMProvider.h"
#include "RagRetriever.h"

enum class StepStatus {
    Ok,
    Failed,
    Cancelled
};

enum class PipelineStatus {
    Idle,
    Running,
    Stopped
};

struct StepResult {
    StepStatus status;
    std::string message;
};

using StepCompleteCallback = std::function<void(StepResult)>;

struct PipelineContext {

    PipelineContext() = default;

    PipelineContext(const PipelineContext&) = delete;
    PipelineContext& operator=(const PipelineContext&) = delete;

    PipelineContext(PipelineContext&&) = delete;
    PipelineContext& operator=(PipelineContext&&) = delete;

    std::atomic<bool> cancelled{ false };
    std::atomic<PipelineStatus> status{ PipelineStatus::Stopped };
    std::string prompt;
	const LLMOutput* llm_result = nullptr;
	LocalAI_EmbeddingResult emb_result;
    std::vector<CapabilityInfo> cap_info;
    std::string response;
    void* user_data = nullptr;
};

class AsyncStep {
public:
    virtual ~AsyncStep() = default;

    virtual void run(
        std::shared_ptr<PipelineContext> ctx,
        StepCompleteCallback on_complete) = 0;

    virtual const char* name() const = 0;
};

class PipelineFinalizer {
public:
    virtual ~PipelineFinalizer() = default;

    virtual Status finalize(
        std::shared_ptr<PipelineContext> ctx,
        void** out_result
    ) = 0;
};

class ChatFinalizer final : public PipelineFinalizer {
public:
    // Inherited via IPipelineFinalizer
    Status finalize(std::shared_ptr<PipelineContext> ctx, void** out_result) override
    {
        size_t size = ctx->response.size() * sizeof(char);
        if (size > 0) {
            char* text_buff = static_cast<char*>(malloc(size));
            if (text_buff) {
                memcpy(text_buff, ctx->response.data(), size);
                *out_result = text_buff;
                return {};
            }
            else {
                return { LOCALAI_OOM, "Failed to malloc text buffer" };
            }
        }
        else {
            fprintf(stderr, "chat response is empty.\n");
            return { LOCALAI_MODEL_GENERATE_ERROR, "chat response empty content."};
        }
    }

};

class EmbeddingStep final : public AsyncStep {
public:
    EmbeddingStep(IEmbeddingProvider* emb) :emb_(emb) {}

    const char* name() const override
    {
        return "Text embedding step (async)";
    }

    void run(std::shared_ptr<PipelineContext> ctx,
        StepCompleteCallback on_complete) override
    {
        auto status = emb_->embedText(ctx->prompt, &ctx->emb_result);
        if (status) {
            on_complete({ StepStatus::Ok, {} });
        }
        else {
            on_complete({ StepStatus::Failed, status.error });
        }
    }

private:
    IEmbeddingProvider* emb_;
};

class RagStep final : public AsyncStep {
public:
    RagStep(RagRetriever* retriever):retriever_(retriever){}

    const char* name() const override
    {
        return "Rag step (async)";
    }

    void run(std::shared_ptr<PipelineContext> ctx,
        StepCompleteCallback on_complete) override
    {
        ctx->cap_info = retriever_->retrieve(ctx->prompt);
        on_complete({ StepStatus::Ok, {}});
    }

private:
    RagRetriever* retriever_ = nullptr;
};

class LLMStep final : public AsyncStep {
public:
    LLMStep(const ILLMProvider* llm) :llm_(llm) {}

    const char* name() const override
    {
        return "LLM genration step (async)";
    }

    void run(std::shared_ptr<PipelineContext> ctx,
        StepCompleteCallback on_complete) override
    {
        llm_->generateAsync(ctx->prompt, std::nullopt, 1024,
            [on_complete, ctx](const LLMOutput* result, const Status& status) {
                if (!status) {
                    on_complete({ StepStatus::Failed, status.error});
                }
                else {
                    ctx->llm_result = result;
                    on_complete({ StepStatus::Ok, {} });
                }
        });
    }

private:
    const ILLMProvider* llm_;
};

class ChatResponseStep final : public AsyncStep {
public:
    ChatResponseStep() {};

    // Inherited via AsyncStep
    void run(std::shared_ptr<PipelineContext> ctx,
        StepCompleteCallback on_complete) override 
    {
        //TODO:
        if (ctx->llm_result) {
            ctx->response = ctx->llm_result->text;
        }
        else {
            ctx->response = {};
        }
        on_complete({ StepStatus::Ok, {} });
    }

    const char* name() const override 
    {
        return "Chat Response Step";
    }
};

class AsyncPipeline : public std::enable_shared_from_this<AsyncPipeline> {
public:
    using AsyncFinalFnCb = std::function<void(const Status &status, void* result)>;
    explicit AsyncPipeline(const char *name)
        : name_(name) {
    }

    void setFinalizer(std::unique_ptr<PipelineFinalizer> finalizer) {
        finalizer_ = std::move(finalizer);
    }

    void addStep(std::unique_ptr<AsyncStep> step) {
        steps_.emplace_back(std::move(step));
    }

    void start(std::shared_ptr<PipelineContext> ctx,
        AsyncFinalFnCb final_cb) 
    {
        status_ = LOCALAI_REQUEST_RUNNING;
        ctx_ = ctx;
        final_cb_ = std::move(final_cb);
        fprintf(stderr, "Start pipeline '%s'.\n", name_? name_:"unnamed");
        runStep(0);
    }

    void cancel() {
        fprintf(stderr, "Cancel pipeline '%s'.\n", name_ ? name_ : "unnamed");
        ctx_->cancelled.store(true, std::memory_order_relaxed);
    }

    LocalAI_RequestStatus status() const {
        return status_;
    }

private:
    void runStep(size_t index) {
        if (ctx_->cancelled) {
            status_ = LOCALAI_REQUEST_CANCELLED;
            completeWithError({ LocalAI_ErrorCode::LOCALAI_CANCELLED, "Pipeline cancelled" });
            return;
        }

        if (index >= steps_.size()) {
            completeWithSuccess();
            return;
        }

        auto self = this->shared_from_this();
        auto& step = steps_[index];

        fprintf(stderr, "Run step '%s'\n", step->name());

        step->run(ctx_, [this, self, index](StepResult r) {
            if (r.status == StepStatus::Ok) {
                runStep(index + 1);
            }
            else {
                completeWithStepError(index, r);
            }
            });
    }

    void completeWithSuccess() 
    {
        status_ = LOCALAI_REQUEST_COMPLETED;
        void* result = nullptr;
        Status st;
        if (finalizer_) {
            st = finalizer_->finalize(ctx_, &result);
        }

        if (final_cb_) {
            final_cb_(st, result);
        }

        fprintf(stderr, "Pipeline '%s' completed with success.\n", name_ ? name_ : "unnamed");
    }

    void completeWithError(const Status& status) 
    {
        status_ = LOCALAI_REQUEST_FAILED;
        if (final_cb_) {
            final_cb_(status, nullptr);
        }
    }

    void completeWithStepError(size_t idx, const StepResult& r) {
        std::string msg =
            std::string("Step failed: ") + steps_[idx]->name() + " - " + r.message;

        fprintf(stderr, "%s", msg.c_str());

        completeWithError({ LocalAI_ErrorCode::LOCALAI_RUNTIME_ERROR, msg });
    }

private:
    std::shared_ptr<PipelineContext> ctx_;
    std::unique_ptr<PipelineFinalizer> finalizer_;
    std::vector<std::unique_ptr<AsyncStep>> steps_;
    AsyncFinalFnCb final_cb_;
    const char* name_;
    std::atomic<LocalAI_RequestStatus> status_{ LOCALAI_REQUEST_PENDING };
};
