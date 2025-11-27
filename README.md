# LocalAI-Assistant
**LocalAI-Assistant** is a local desktop AI assistant built with C++/Qt.   It supports voice and text input to perform local tasks.   All models run locally — no user data is uploaded, ensuring full privacy.   A plugin-based architecture allows easy expansion of new capabilities.

## Key Features

- **Local Speech Recognition** (Whisper.cpp)
- **Local LLM Conversation** (llama.cpp)
- **Plugin-Based Function Calls**:
  - Open applications
  - Search files
  - Set reminders
- **System Control Functions** (open programs, search files, reminders, web)
- **Modular architecture**: plugins can be developed and loaded independently

## Architecture
<img width="1024" height="1536" alt="localAI-Assistant-Architecture" src="https://github.com/user-attachments/assets/1dfa1155-105f-407e-b9d8-401da4cb1fdc" />

## Plugin Design

Each plugin (DLL) registers its capabilities with the assistant system:

```json
{
  {
    "id": "com.yoann.simple_plugin",
    "version": "1.0.0",
    "platforms": [
        "windows"
    ],
    "description": "A plugin that can open files on Windows.",
    "author": "Yoann",
    "entry": "simple_plugin",
    "capabilities": [
        {
            "name": "filesystem.create_directory",
            "description": "Create a new directory at the specified path",
            "parameters": [
                {
                    "name": "path",
                    "type": "string",
                    "required": true
                }
            ]
        },
        {
            "name": "filesystem.create_file",
            "description": "Create a new file at the specified path",
            "parameters": [
                {
                    "name": "path",
                    "type": "string",
                    "required": true
                }
            ]
        },
        {
            "name": "filesystem.write_file",
            "description": "Write content to a file at the specified path",
            "parameters": [
                {
                    "name": "path",
                    "type": "string",
                    "required": true
                },
                {
                    "name": "content",
                    "type": "string",
                    "required": true
                }
            ]
        },
        {
            "name": "filesystem.search_files",
            "description": "Search for files in a directory matching a given pattern",
            "parameters": [
                {
                    "name": "directory_path",
                    "type": "string",
                    "required": true
                },
                {
                    "name": "pattern",
                    "type": "string",
                    "required": true
                }
            ]
        }
    ],
    "intents": [
        {
            "intent_name": "create_project",
            "description": "Create a project folder and main file",
            "actions": [
                {
                    "name": "filesystem.create_directory"
                },
                {
                    "name": "filesystem.create_file"
                },
                {
                    "name": "filesystem.write_file"
                }
            ]
        },
        {
            "intent_name": "clear_temp_files",
            "description": "Delete all temporary files in a specified directory",
            "actions": [
                {
                    "name": "filesystem.search_files"
                },
                {
                    "name": "filesystem.delete_file"
                }
            ]
        }
    ]
}
}
