{
  "targets": [
    {
      "target_name": "core",
      "sources": [
        "./src/collection/collection.cpp",
        "./src/collection/index.cpp",
        "./src/collection/query.cpp",
        "./src/file/page_manager.cpp",
        "./src/file/storage_page.cpp",
        "./src/main.cpp",
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      "dependencies": [
        "<!(node -p \"require('node-addon-api').gyp\")"
      ],
      "cflags!": ["-fno-exceptions"],
      "cflags_cc!": ["-fno-exceptions"],
      "defines": ["NAPI_CPP_EXCEPTIONS"],
      "xcode_settings": {
        "GCC_ENABLE_CPP_EXCEPTIONS": "YES"
      },
      'msvs_settings': {
        'VCCLCompilerTool': {
          'AdditionalOptions': [ '-std:c++17', ],
        },
      },
    }
  ]
}