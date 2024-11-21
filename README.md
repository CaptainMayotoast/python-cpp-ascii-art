## ASCII art with C++, CUDA and Python

##### Update submodules (for the Docker image repo)

1. Run `git submodule update --init`

2. Run `git submodule update --recursive --remote` in the top level development folder (i.e. `/build`)
---

##### Getting started

1. Build the C++ development container, run `cpp-development-container/gcc13/assemble_images_docker.sh`.

2. Enter the devcontainer with VS Code.

3. Build with: `./build.sh <debug|release>`.

### CUDA

The GCC build container referenced in this repo copies libraries and headers from a prebuilt CUDA container.

On the host, follow [this](https://docs.nvidia.com/datacenter/cloud-native/container-toolkit/latest/install-guide.html#installing-with-apt) NVidia guide to install the NVidia Container Toolkit. 

Then, run:

1. `sudo apt-get install -y nvidia-container-toolkit`

2. `sudo nvidia-ctk runtime configure --runtime=docker`

3. `sudo systemctl restart docker`

### Tests

Run `meson test -C <build type>`
