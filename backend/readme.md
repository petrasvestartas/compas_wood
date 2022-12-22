# compas_wood backend

## CMake

```cmake
##### Step 1: Create a build directory and cd into it
# cd C:\IBOIS57\_Code\Software\Python\compas_wood\backend\build_win
##### Step 2: Run CMake to download libraries
# cmake --fresh -DGET_LIBS=ON -DCOMPILE_LIBS=OFF -DBUILD_MY_PROJECTS=OFF -DRELEASE_DEBUG=ON -DCMAKE_BUILD_TYPE="Release"  -G "Visual Studio 17 2022" -A x64 .. && cmake --build . --config Release 
##### Step 3: Run CMake to build 3rd-party libraries
# cmake --fresh -DGET_LIBS=OFF -DBUILD_MY_PROJECTS=ON -DCOMPILE_LIBS=ON -DRELEASE_DEBUG=ON -DCMAKE_BUILD_TYPE="Release"  -G "Visual Studio 17 2022" -A x64 .. && cmake --build . --config Release 
##### Step 4: Run CMake to build the code
# cmake --fresh -DGET_LIBS=OFF -DBUILD_MY_PROJECTS=ON -DCOMPILE_LIBS=OFF -DRELEASE_DEBUG=ON -DCMAKE_BUILD_TYPE="Release"  -G "Visual Studio 17 2022" -A x64 .. && cmake --build . --config Release 
##### Step 5 - Run the code - you can use this as a default VScode task:
# cmake  --build C:\\IBOIS57\\_Code\\Software\\Python\\compas_wood\\backend\\build_win\\ -v --config Release --parallel 4 &&  C:\\IBOIS57\\_Code\\Software\\Python\\compas_wood\\backend\\build_win\\Release\\wood.exe
```

## tasks

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

## Commit PREFIX rules

```
ADD "" - new feature
FIX "" - bug fix
DOC "" - documentation
TEST "" - tests
TUT "" - tutorials
MISC "" - unknown
```

# git 

## Add files to the repository
```
git add --all
git commit -m "..."
git push origin backend
```

## git branch

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

## cpp intellisence

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
