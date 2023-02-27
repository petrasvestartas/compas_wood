# wood frontend

### How to prepare the project from backend

There are 3 main steps:
* prepare precompiled header "stdafx.h"
* Copy the whole "src/wood" folder from backend to frontend.
* Write CPP and Python files in "src/wood_pybind11"


### Superbuild

Use "cmake_bash" folder to build the project.

## Visual Studio Code Shortcuts
```

CODING HELP
Ctrl + Space   =   Autocomplete using IntelliSense.
CTRL + Enter   =   Copilot on the right side of the window

SELECTION
Shift + End   =   Select the entire line from start to end.
Shift + Home   =   Select the entire line from end to start.
Alt +Shift +arrow keys(←,↑,↓,→)   =  Select custom part of the code.

BUILD
Ctrl + Shift + B   =   Build your project.

COMMENTS
Ctrl + K, Ctrl + C   =   Comments the selected lines.
Ctrl + K, Ctrl + U   =   Uncomments the selected lines.
Ctrl + K, Ctrl + D   =   Do proper alignment of all the code.


```



## GIT 

### Commit PREFIX rules

```
ADD "" - new feature
FIX "" - bug fix
DOC "" - documentation
TEST "" - tests
TUT "" - tutorials
MISC "" - unknown
```

### Add files to the repository
```
git add --all
git commit -m "..."
git push origin backend
```

### git branch

To create a new branch and switch to it at the same time:
```
git checkout -b backend
```

Show the current branch you are in:
```
git branch --show-current
```

Show all the branches:
```
git branch --show-current
```

## VISUAL STUDIO CODE

### tasks

```json
{
	"version": "2.0.0",
	"tasks": [
		{
			"type": "shell",
			"label": "BUILD ",
			//"command": " cmake  --build ${fileDirname}\\build_win\\ -v --config Release  &&  ${fileDirname}\\build_win\\Release\\wood.exe",//--config Release 
			"command": " cmake  --build C:\\IBOIS57\\_Code\\Software\\Python\\compas_wood\\backend\\build_win\\ -v --config Release --parallel 4 &&  C:\\IBOIS57\\_Code\\Software\\Python\\compas_wood\\backend\\build_win\\Release\\wood.exe",
			// "problemMatcher": [
			// 	"$gcc"
			// ],
			"detail": "compiler: msvc",
			"group": {
				"kind": "build",
				"isDefault": true
			},
		},
		{
			"type": "shell",
			"label": "REBUILD",
			//"command": " cmake  --build ${fileDirname}\\build_win\\ -v --config Release  &&  ${fileDirname}\\build_win\\Release\\wood.exe",
			"command": "cmake --fresh -DGET_LIBS=OFF -DBUILD_MY_PROJECTS=ON -DRELEASE_DEBUG=ON -DCMAKE_BUILD_TYPE=\"Release\"  -G \"Visual Studio 17 2022\" -A x64 C:\\IBOIS57\\_Code\\Software\\Python\\compas_wood\\backend\\build_win\\  && cmake --build C:\\IBOIS57\\_Code\\Software\\Python\\compas_wood\\backend\build_win\\ --config Release  &&  C:\\IBOIS57\\_Code\\Software\\Python\\compas_wood\\backend\\build_win\\Release\\wood.exe",
			"problemMatcher": [
				"$gcc"
			],
			"detail": "compiler: msvc",
			"group": {
				"kind": "build",
				"isDefault": false
			}
		},
		{
			"type": "shell",
			"label": "DOWNLOAD LIBRARIES",
			//"command": " cmake  --build ${fileDirname}\\build_win\\ -v --config Release  &&  ${fileDirname}\\build_win\\Release\\wood.exe",
			"command": "cmake --fresh -DGET_LIBS=ON -DBUILD_MY_PROJECTS=OFF -DRELEASE_DEBUG=ON -DCMAKE_BUILD_TYPE=\"Release\"  -G \"Visual Studio 17 2022\" -A x64 C:\\IBOIS57\\_Code\\Software\\Python\\compas_wood\\backend\\  && cmake --build C:\\IBOIS57\\_Code\\Software\\Python\\compas_wood\\backend\\build_win\\ --config Release  &&  C:\\IBOIS57\\_Code\\Software\\Python\\compas_wood\\backend\\build_win\\Release\\wood.exe",
			"problemMatcher": [
				"$gcc"
			],
			"detail": "compiler: msvc",
			"group": {
				"kind": "build",
				"isDefault": false
			}
		},

	]
}

```

### c_cpp_properties

``` json
{
    "configurations": [
        {
            "browse": {
                "databaseFilename": "",
                "limitSymbolsToIncludedHeaders": true
            },
            "includePath": [
                "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/",
                "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/src/viewer/include/",
                "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/src/viewer/include/",
                "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/src/viewer/include/cdt/",
                "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/src/viewer/include/imgui/",
                "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/src/viewer/include/opengl/",
                "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/src/viewer/include/pipe/",
                "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/src/viewer/include/ply/",
                "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/src/viewer/include/stb/",
                "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/src/wood/include/",
                "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/src/nest/",
                "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/cdt/CDT/include/",
                "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/clipper_2/CPP/Clipper2Lib/include/",
                "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/imgui/",
                "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/imgui/backends/",
                "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/glfw/include/",
                "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/glad/include/",
                "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/glm/",
                "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/glm/glm/",
                "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/glm/glm/detail/",
                "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/glm/glm/ext/",
                "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/glm/glm/gtc/",
                "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/glm/glm/gtx/",
                "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/glm/glm/simd/",
                "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/assimp/include/",
                "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/cgal/include/",
                "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/eigen/Eigen/",
                "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/eigen/",
                "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/boost/include/boost-1_78/",
                "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/googletest/include/",
                "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/googletest/include/gtest/",
                "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/googletest/include/gtest/internal/",
                "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/googletest/include/gtest/custom/",
                "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/googletest/include/gmock/",
                "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/googletest/include/gmock/internal/",
                "C:/IBOIS57/_Code/Software/Python/compas_wood/backend/build_win/install/googletest/include/gmock/custom/"
            ],
            "name": "wood_config",
            "intelliSenseMode": "gcc-x64",
            "compilerPath": "C:\\Ninja\\ninja.exe",
            "cStandard": "c17",
            "cppStandard": "c++20",
            "compilerArgs": [
                ""
            ],
            "configurationProvider": "ms-vscode.makefile-tools"
        }
    ],
    "version": 4
}
```


### launch
``` json
//https://www.youtube.com/watch?v=Rfj40xW9q6w
{
  "version": "0.2.0",
  "configurations": [
    {
      "name": "Debug",
      "type": "cppdbg",
      "request": "launch",
      "cwd": "${fileDirname}",
      "miDebuggerPath": "gdb",
      "program": "${workspaceRoot}/build_win/Release/wood",
      "args": [],
    }
  ]
}
```
### settings
``` json
{
  "python.linting.flake8Enabled": true,
  "python.linting.enabled": true,
  "C_Cpp_Runner.cCompilerPath": "C:/Ninja/ninja.exe",
  "C_Cpp_Runner.cppCompilerPath": "g++",
  "C_Cpp_Runner.debuggerPath": "gdb",
  "C_Cpp_Runner.cStandard": "gnu17",
  "C_Cpp_Runner.cppStandard": "",
  "C_Cpp_Runner.msvcBatchPath": "",
  "C_Cpp_Runner.useMsvc": false,
  "C_Cpp_Runner.warnings": [
    "-Wall",
    "-Wextra",
    "-Wpedantic"
  ],
  "C_Cpp_Runner.enableWarnings": true,
  "C_Cpp_Runner.warningsAsError": false,
  "C_Cpp_Runner.compilerArgs": [],
  "C_Cpp_Runner.linkerArgs": [],
  "C_Cpp_Runner.includePaths": [],
  "C_Cpp_Runner.includeSearch": [
    "*",
    "**/*"
  ],
  "C_Cpp_Runner.excludeSearch": [
    "**/build",
    "**/build/**",
    "**/.*",
    "**/.*/**",
    "**/.vscode",
    "**/.vscode/**"
  ],
  "files.associations": {
    "limits": "cpp",
    "fstream": "cpp",
    "numeric": "cpp",
    "string": "cpp",
    "algorithm": "cpp",
    "array": "cpp",
    "cmath": "cpp",
    "cstdarg": "cpp",
    "cstddef": "cpp",
    "cstdint": "cpp",
    "cstdio": "cpp",
    "cstdlib": "cpp",
    "cstring": "cpp",
    "ctime": "cpp",
    "cwchar": "cpp",
    "deque": "cpp",
    "exception": "cpp",
    "functional": "cpp",
    "initializer_list": "cpp",
    "iomanip": "cpp",
    "ios": "cpp",
    "iosfwd": "cpp",
    "iostream": "cpp",
    "istream": "cpp",
    "iterator": "cpp",
    "list": "cpp",
    "map": "cpp",
    "memory": "cpp",
    "new": "cpp",
    "ostream": "cpp",
    "random": "cpp",
    "sstream": "cpp",
    "stack": "cpp",
    "stdexcept": "cpp",
    "streambuf": "cpp",
    "system_error": "cpp",
    "tuple": "cpp",
    "type_traits": "cpp",
    "typeinfo": "cpp",
    "unordered_map": "cpp",
    "unordered_set": "cpp",
    "utility": "cpp",
    "vector": "cpp",
    "xfacet": "cpp",
    "xhash": "cpp",
    "xiosbase": "cpp",
    "xlocale": "cpp",
    "xlocinfo": "cpp",
    "xlocmon": "cpp",
    "xlocnum": "cpp",
    "xloctime": "cpp",
    "xmemory": "cpp",
    "xmemory0": "cpp",
    "xstddef": "cpp",
    "xstring": "cpp",
    "xtr1common": "cpp",
    "xtree": "cpp",
    "xutility": "cpp",
    "chrono": "cpp",
    "ratio": "cpp",
    "queue": "cpp",
    "set": "cpp",
    "xthread": "cpp",
    "thread": "cpp",
    "atomic": "cpp",
    "bitset": "cpp",
    "cctype": "cpp",
    "cfenv": "cpp",
    "complex": "cpp",
    "locale": "cpp",
    "mutex": "cpp",
    "optional": "cpp",
    "string_view": "cpp",
    "strstream": "cpp",
    "typeindex": "cpp",
    "variant": "cpp",
    "xlocbuf": "cpp",
    "xlocmes": "cpp",
    "clocale": "cpp",
    "csetjmp": "cpp",
    "csignal": "cpp",
    "cwctype": "cpp",
    "execution": "cpp"
  },
  "C_Cpp.dimInactiveRegions": false,
  "git.ignoreLimitWarning": true,
  "cSpell.words": [
    "Minkowski"
  ],
  "workbench.colorCustomizations": {
    "activityBar.background": "#000000",
    "statusBar.background": "#333",
    "sideBar.foreground": "#606060",
    "sideBar.background": "#000000",
    "titleBar.activeBackground": "#000000",
    "terminal.background": "#000000"
  }

  "editor.tokenColorCustomizations": {
    "textMateRules": [
      {
        "scope": [
          "constant",
          "constant.character",
          "constant.character.escape",
          "constant.numeric",
          "constant.numeric.integer",
          "constant.numeric.float",
          "constant.numeric.hex",
          "constant.numeric.octal",
          "constant.other",
          "constant.regexp",
          "constant.rgb-value",
          "emphasis",
          "entity",
          "entity.name",
          "entity.name.class",
          "entity.name.function",
          "entity.name.method",
          "entity.name.section",
          "entity.name.selector",
          "entity.name.tag",
          "entity.name.type",
          "entity.other",
          "entity.other.attribute-name",
          "entity.other.inherited-class",
          "invalid",
          "invalid.deprecated",
          "invalid.illegal",
          "keyword",
          "keyword.control",
          "keyword.operator",
          "keyword.operator.new",
          "keyword.operator.assignment",
          "keyword.operator.arithmetic",
          "keyword.operator.logical",
          "keyword.other",
          "markup",
          "markup.bold",
          "markup.changed",
          "markup.deleted",
          "markup.heading",
          "markup.inline.raw",
          "markup.inserted",
          "markup.italic",
          "markup.list",
          "markup.list.numbered",
          "markup.list.unnumbered",
          "markup.other",
          "markup.quote",
          "markup.raw",
          "markup.underline",
          "markup.underline.link",
          "meta",
          "meta.block",
          "meta.cast",
          "meta.class",
          "meta.function",
          "meta.function-call",
          "meta.preprocessor",
          "meta.return-type",
          "meta.selector",
          "meta.tag",
          "meta.type.annotation",
          "meta.type",
          "punctuation.definition.string.begin",
          "punctuation.definition.string.end",
          "punctuation.separator",
          "punctuation.separator.continuation",
          "punctuation.terminator",
          "storage",
          "storage.modifier",
          "storage.type",
          "string",
          "string.interpolated",
          "string.other",
          "string.quoted",
          "string.quoted.double",
          "string.quoted.other",
          "string.quoted.single",
          "string.quoted.triple",
          "string.regexp",
          "string.unquoted",
          "strong",
          "support",
          "support.class",
          "support.constant",
          "support.function",
          "support.other",
          "support.type",
          "support.type.property-name",
          "support.variable",
          "variable",
          "variable.language",
          "variable.name",
          "variable.other",
          "variable.other.readwrite",
          "variable.parameter"
        ],
        "settings": {
          "fontStyle": "bold"
        }
      },
      {
        "scope": [
          "comment",
          "punctuation.definition.comment"
        ],
        "settings": {
          "fontStyle": "italic",
          "foreground": "#000000",
        }
      },
      {
        "scope": [
          "constant",
          "constant.character",
          "constant.character.escape",
          "constant.numeric",
          "constant.numeric.integer",
          "constant.numeric.float",
          "constant.numeric.hex",
          "constant.numeric.octal",
          "constant.other",
          "constant.regexp",
          "constant.rgb-value",
          "storage",
          "storage.modifier",
          "storage.type",
          "string",
          "string.interpolated",
          "string.other",
          "string.quoted",
          "string.quoted.double",
          "string.quoted.other",
          "string.quoted.single",
          "string.quoted.triple",
          "string.regexp",
          "string.unquoted",
        ],
        "settings": {
          "foreground": "#003cff",
        }
      },
      {
        "scope": [
          "variable",
          "variable.language",
          "variable.name",
          "variable.other",
          "variable.other.readwrite",
          "variable.parameter"
        ],
        "settings": {
          "foreground": "#d000b7",
        }
      },
      {
        "scope": [
          "storage",
          "storage.modifier",
          "storage.type",
          "meta",
          "meta.block",
          "meta.cast",
          "meta.class",
          "meta.function",
          "meta.function-call",
          "meta.preprocessor",
          "meta.return-type",
          "meta.selector",
          "meta.tag",
          "meta.type.annotation",
          "meta.type",
        ],
        "settings": {
          "foreground": "#000000",
        }
      }
    ]
  },
}
