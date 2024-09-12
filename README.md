## Building C++ with Meson

1.  Run `docker run -u "$(id -u)":"$(id -g)" -v /etc/passwd:/etc/passwd -v /etc/group:/etc/group -v $PWD/:/build -it --rm meson-build:latest <build mode>` on the root of this repo,

    OR

    run `/build.sh <arg>` from within VS Code container development environment (`build <arg>` if `.devcontainer/.bashrc` has been sourced).

> The entrypoint accepts the last argument(s) to `docker run ...`.

> Choose between build modes: `release`, `debug`, `debugsan`, `coverage` and all aforementioned modes have a version of clean by appending `-clean` to them (i.e. `debug-clean`).

2. Profit 💯 
 
## Updating submodules (for the Docker image repo)

1. Run `git submodule update --init`

2. Run `git submodule update --recursive --remote` in the top level development folder (i.e. `/build`)
> Submodules are a little weird...the command in #2 may not always update things without first issuing a `--init` variant of that command.

## Add Meson subprojects (library exists in [WrapDB](https://mesonbuild.com/Wrapdb-projects.html))

1. Check if the desired software package exists with Meson.  Run `meson wrap list`.

2. If it does, run `meson wrap install <name>`

3. Specify the subproject in the highest level meson.build file, for example: `fmt_proj = subproject('fmt')`.

4. Specify the [dependency](https://mesonbuild.com/Dependencies.html#dependencies): `fmt_dep = fmt_proj.get_variable('fmt_dep')`.
> Important notes: investigate the git repo for CMakeLists.txt or meson.build to identify what text should come before "_proj".  In the `fmt` example, CMakeLists.txt [specifies](https://github.com/fmtlib/fmt/blob/master/CMakeLists.txt#L66) the project name as `FMT`.  Notice that the `get_variable` argument is also the same as that which is found within the .wrap file, under section `provide`, to the right of the `=`.

## Add Meson subprojects (library uses Meson to build AND library does not exist in [WrapDB](https://mesonbuild.com/Wrapdb-projects.html))

1. Create a `.wrap` file under `subprojects` folder.
> Note: `subprojects` needs to be named this for the sake of Meson.

2. `pistache.wrap` is an example of a subproject that is git driven, and `pistache` happens to use Meson, which makes things easier.  Review this file as an example.

3. Follow the repo README on Meson specific instructions, or, if that does not exist, specify the subproject in the highest level meson.build file, for example: `fmt_proj = subproject('fmt')`.

4. Specify the [dependency](https://mesonbuild.com/Dependencies.html#dependencies): `fmt_dep = fmt_proj.get_variable('fmt_dep')`.

## Add Meson subprojects (that happen to be CMake build based)

1. Follow the [Meson documentation](https://mesonbuild.com/CMake-module.html#cmake-subprojects) for this.

## Speed up builds with ccache (unnecessary in most cases, as Meson + Ninja is FAST)

1. [Read](https://mesonbuild.com/Feature-autodetection.html#ccache).

2. Install [ccache](https://ccache.dev/) within a dev container or on a host where the code will be built

## Docker config

Run `sudo usermod -aG docker <user>` to add the `<user>` to the `docker` group.

### CUDA

The GCC build container referenced in this repo copies libraries and headers from a prebuilt CUDA container.

On the host, follow [this](https://docs.nvidia.com/datacenter/cloud-native/container-toolkit/latest/install-guide.html#installing-with-apt) NVidia guide to install the NVidia Container Toolkit. 

Then, run:

1. `sudo apt-get install -y nvidia-container-toolkit`

2. `sudo nvidia-ctk runtime configure --runtime=docker`

3. `sudo systemctl restart docker`
