## Docker development container
We recognize that recreating development environment for this project may be complicated, especially on unsupported platforms. To ease things for our developers and contributors, we created a stable, reproducible environment dubbed "Dev container". It can be used in any IDE or in a standalone manner, but is fully integrated with Visual Studio Code. 
## Usage
### VSCode
If you use VSCode as your code editor of choice, things are simple: install Docker, install `Remote - Containers` extension for VSCode, and reload the window. You should see a prompt to reopen the project in container (if it didn't appear, you can fire up `Reopen in Container` action in Command Palette). First load/build may take a while, so don't rush to cancel the process. After the window is reopened, you can open terminals, edit code and recompile the project as usual.

### JetBrains IDE
You can add our development environment container as a Docker toolchain to your JetBrains IDE. Check [this guide](https://www.jetbrains.com/help/clion/clion-toolchains-in-docker.html) for more info.

### Standalone
Using this image directly in the terminal is also possible:

1. To build it, use `devcontainer` target in our Dockerfile
```
docker build . -t ostis/sc-machine-builder  --target=devcontainer
```

2. Run the container mounting your workspace folder to `/sc-machine` inside the container. Optionally you can create a volume for `ccache` to preserve build cache across restarts:
```
docker run --rm -i -t -v sc-machine_ccache:/ccache -v < path to the project folder>:/sc-machine ostis/sc-machine-builder
```