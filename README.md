# This is our Chess game semestral project

This project is written in C++

## Requirements

Below are the recommended requirements for developing and building this project.

- Host (recommended):
	- Docker Engine (preferably modern version, e.g. 20.10+)
	- Docker Compose (plugin v2) or the `docker compose` command
	- Git (to clone/update the repository)
	- (Optional) native toolchain if you prefer not to use Docker: a recent C++ toolchain (g++/clang), CMake and Ninja

- Inside the development container (provided by the project):
	- g++ (GNU C++ compiler)
	- CMake (>= 3.15 recommended)
	- Ninja (build backend)

## Quick Docker dev container usage

The repository includes a development image you can build and run with Docker Compose. The project directory is mounted into the container at `/workspace`.

Build the container:
```powershell
docker compose build
```

Start the container in the backgorund:

```powershell
docker compose up --build -d dev
```
Option 1: Go inside the container:

```powershell
docker exec -it $(docker compose ps -q dev) bash
```

Option 2: Open shell inside the container - only one time shell:

```powershell
docker compose run --rm dev
```

Inside the container, a typical out-of-source build with CMake + Ninja (debug):

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build
```

Tests (optional, if you have ctest):

```bash
ctest --test-dir build --output-on-failure
```

Instant build and run - one time use container:
```bash 
docker compose run --rm dev bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/<exe_name>  
```

One line:
```bash
docker compose run --rm dev bash -lc "cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug && cmake --build build && ./build/<exe>"
```

# Compiling using only g++:

Build the container:
```powershell
docker compose build
```

```powershell
docker compose run --rm dev
```

```bash
g++ src/main.cpp -o main.out
./main.out
```



Notes:

- If you don't have Docker available, install a native toolchain that provides the tools listed above.