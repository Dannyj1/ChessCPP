FROM mcr.microsoft.com/devcontainers/cpp:1-debian-12

# [Optional] Uncomment this section to install additional vcpkg ports.
# RUN su vscode -c "${VCPKG_ROOT}/vcpkg install <your-port-name-here>"

# [Optional] Uncomment this section to install additional packages.
ENV CC=clang
ENV CXX=clang++
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
     && apt install lsb-release wget software-properties-common gnupg git -y \
     && wget https://apt.llvm.org/llvm.sh  \
     && chmod +x llvm.sh \
     && ./llvm.sh 18 \
     && apt install llvm-18-dev libclang-18-dev clang-18 -y \
     && update-alternatives --install /usr/bin/cc cc /usr/bin/clang-18 100 \
     && update-alternatives --install /usr/bin/c++ c++ /usr/bin/clang++-18 100 \
     && update-alternatives --install /usr/bin/clang clang /usr/bin/clang-18 100 \
     && update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-18 100

RUN git clone https://github.com/include-what-you-use/include-what-you-use.git
WORKDIR include-what-you-use
RUN git checkout clang_18
WORKDIR ..
RUN mkdir build && cd build
RUN cmake -G "Unix Makefiles" -DCMAKE_PREFIX_PATH=/usr/lib/llvm-18 ./include-what-you-use
RUN make -j4
RUN make install
