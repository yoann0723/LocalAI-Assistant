#include "core_c_api.h"

#include <string>

struct LocalAI_Status_t {
    LocalAI_ErrorCode code;
    char msg[];
};

LocalAI_Status* LocalAI_CreateStatus(LocalAI_ErrorCode code, const char* msg) {
    if (!msg) msg = "";

    size_t len = strlen(msg);
    LocalAI_Status* st = (LocalAI_Status*)malloc(sizeof(LocalAI_Status) + len + 1);
    if (!st) return NULL;

    st->code = code;
    memcpy(st->msg, msg, len);
    st->msg[len] = '\0';
    return st;
}

LocalAI_ErrorCode LocalAI_StatusGetCode(const LocalAI_Status* st) {
    return st ? st->code : LocalAI_ErrorCode::LOCALAI_UNKNOWN;
}

const char* LocalAI_StatusGetMessage(const LocalAI_Status* st) {
    return st ? st->msg : "";
}

void LocalAI_StatusDestroy(LocalAI_Status* st) {
    if (st) free(st);
}