# LocalAI-Assistant
**LocalAI-Assistant** is a local desktop AI assistant built with C++/Qt.   It supports voice and text input to perform local tasks.   All models run locally — no user data is uploaded, ensuring full privacy.   A plugin-based architecture allows easy expansion of new capabilities.

## Key Features

- **Local Speech Recognition** (Whisper ONNX model)
- **Local LLM Conversation** (Qwen2-1.5B-int4)
- **Plugin-Based Function Calls**:
  - Open applications
  - Search files
  - Set reminders
- **Local TTS (optional)**
- **System Control Functions** (open programs, search files, reminders, web)
- **Modular architecture**: plugins can be developed and loaded independently

## Plugin Design

Each plugin (DLL) registers its capabilities with the assistant system:

```json
{
  "name": "open_app",
  "description": "Open a desktop application",
  "parameters": {
    "app_name": "string"
  }
}
