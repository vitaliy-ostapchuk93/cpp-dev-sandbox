# Versions

ARG UBUNTU_VERSION=24.04
ARG GCC_VERSION=14
ARG LLVM_VERSION=17

# Base Image
FROM docker.io/library/ubuntu:${UBUNTU_VERSION} AS base

# +-----------------------------+
# | REUSE GLOBAL ARGS           |
# +-----------------------------+

ARG UBUNTU_VERSION
ARG GCC_VERSION
ARG LLVM_VERSION

# +-----------------------------+
# | PRE-REQUISITE/INIT PACKAGES |
# +-----------------------------+

RUN apt-get update && apt-get install -y locales && rm -rf /var/lib/apt/lists/* \
	&& localedef -i en_US -c -f UTF-8 -A /usr/share/locale/locale.alias en_US.UTF-8

ENV LANG en_US.utf8
ENV DEBIAN_FRONTEND=noninteractive

WORKDIR /tmp

# Install Packages
RUN apt-get update && apt-get install -y --no-install-recommends \
    gnupg2 ca-certificates software-properties-common \
    cppcheck valgrind ccache cmake wget git nano jq \
    # required by Visual Studio
    gdb make ninja-build rsync zip sudo \
    # required by VCPKG
    openssh-server tar curl unzip pkg-config bash-completion aria2 mono-complete \
    # required by LLVM
    lsb-release zlib1g-dev \
    # shell
    zsh


# +-----------------------------+
# | Github CLI                  |
# +-----------------------------+

RUN curl -fsSL https://cli.github.com/packages/githubcli-archive-keyring.gpg | dd of=/usr/share/keyrings/githubcli-archive-keyring.gpg && \
    chmod go+r /usr/share/keyrings/githubcli-archive-keyring.gpg && \
    echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/githubcli-archive-keyring.gpg] https://cli.github.com/packages stable main" | tee /etc/apt/sources.list.d/github-cli.list > /dev/null && \
    apt-get update && apt-get install -y --no-install-recommends gh -y

# +-----------------------------+
# | CMake                       |
# +-----------------------------+

RUN version="$(curl -s https://api.github.com/repos/Kitware/CMake/releases/latest | jq -r '.tag_name' | cut -c 2-)"; echo "Latest Version: $version" && \
    url="https://github.com/Kitware/CMake/releases/download/v$version/cmake-$version-linux-x86_64.sh"; echo "Download URL: $url" && \
    wget -q $url && \
    mkdir /opt/cmake && \
    sh cmake-$version-linux-x86_64.sh --prefix=/opt/cmake --skip-license && \
    ln -s /opt/cmake/bin/cmake /usr/local/bin/cmake && \
    rm -rf cmake-$version-linux-x86_64.sh


# +-----------------------------+
# | GCC                         |
# +-----------------------------+

RUN add-apt-repository universe && \
    apt-get update && apt-get install -y --no-install-recommends \
    software-properties-common \
    libmpfr-dev libgmp3-dev libmpc-dev \
    gcc-${GCC_VERSION} g++-${GCC_VERSION}

RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-${GCC_VERSION} 100 && \
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-${GCC_VERSION} 100

RUN apt-get autoremove -y libmpfr-dev libgmp3-dev libmpc-dev

# +-----------------------------+
# | LLVM                        |
# +-----------------------------+

RUN wget https://apt.llvm.org/llvm.sh && \
    chmod +x llvm.sh && \
    ./llvm.sh ${LLVM_VERSION} all && \
    rm ./llvm.sh

# add llvm to path
ENV PATH="/usr/lib/llvm-${LLVM_VERSION}/bin:/usr/lib/llvm-${LLVM_VERSION}/include:${PATH}"
ENV LD_LIBRARY_PATH="/usr/lib/llvm-${LLVM_VERSION}/lib:${LD_LIBRARY_PATH}"

# unversionize the binaries
RUN ln -s /usr/bin/clang-${LLVM_VERSION} /usr/bin/clang && \
    ln -s /usr/bin/clang++-${LLVM_VERSION} /usr/bin/clang++ && \
    ln -s /usr/bin/clang-format-${LLVM_VERSION} /usr/bin/clang-format

# update compiler environment vars
ENV CC=/usr/bin/clang \
    CXX=/usr/bin/clang++

# update alternatives
RUN update-alternatives --install /usr/bin/cc cc /usr/bin/clang 100 && \
    update-alternatives --install /usr/bin/c++ c++ /usr/bin/clang++ 100


# +-----------------------------+
# | ZSH                         |
# +-----------------------------+
# setup zsh, omyzsh, powerline fonts
# setup zsh plugins: autosuggestions, autocompletions, history search
# setup starship

ENV TERM=xterm-256color \
    ZSH_THEME=agnoster

RUN apt-get update && apt-get install -y --no-install-recommends \
    zsh powerline fonts-powerline && \
    curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh | sh && \
    git clone --depth=1 https://github.com/zsh-users/zsh-autosuggestions ${ZSH_CUSTOM:-~/.oh-my-zsh/custom}/plugins/zsh-autosuggestions && \
    git clone --depth=1 https://github.com/zsh-users/zsh-completions ${ZSH_CUSTOM:=~/.oh-my-zsh/custom}/plugins/zsh-completions && \
    git clone --depth=1 https://github.com/zsh-users/zsh-history-substring-search ${ZSH_CUSTOM:-~/.oh-my-zsh/custom}/plugins/zsh-history-substring-search && \
    sed -i "s/plugins=(git)/plugins=(git zsh-completions zsh-autosuggestions zsh-history-substring-search)/" /root/.zshrc && \
    sed -i "s/ZSH_THEME=\"agnoster\"/ZSH_THEME=\"robbyrussell\"/" /root/.zshrc

ENV SHELL /bin/zsh

RUN sh -c "$(curl -fsSL https://starship.rs/install.sh)" -- --yes && \
    echo 'eval "$(starship init bash)"' >> ~/.bashrc && \
    echo 'eval "$(starship init zsh)"' >> /root/.zshrc


# +-----------------------------+
# | vcpkg                       |
# +-----------------------------+

ENV VCPKG_ROOT=/opt/vcpkg \
    VCPKG_INSTALLATION_ROOT=/opt/vcpkg \
    VCPKG_TRIPLET=x64-linux \
    VCPKG_FORCE_SYSTEM_BINARIES=1 \
    VCPKG_USE_SYSTEM_BINARIES=1
ENV PATH="${PATH}:${VCPKG_ROOT}"

RUN git clone --depth=1 \
    -c core.eol=lf \
    -c core.autocrlf=false \
    -c fsck.zeroPaddedFilemode=ignore \
    -c fetch.fsck.zeroPaddedFilemode=ignore \
    -c receive.fsck.zeroPaddedFilemode=ignore \
    https://github.com/microsoft/vcpkg "${VCPKG_ROOT}" && \
    "${VCPKG_ROOT}"/bootstrap-vcpkg.sh -disableMetrics && \
    "${VCPKG_ROOT}"/vcpkg integrate install


# +-----------------------------+
# | CLEANUP                     |
# +-----------------------------+

RUN apt-get autoremove -y && \
    apt-get clean autoclean && \
    rm -rf /var/lib/apt/lists/* \
    rm -rf /tmp/*

# +-----------------------------+
# | Set Locale + Timezone       |
# +-----------------------------+

ENV TZ=Europe/Berlin

# configure german locale
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && \
    echo $TZ > /etc/timezone && \
    dpkg-reconfigure tzdata && \
    sed -i -e 's/# en_US.UTF-8 UTF-8/en_US.UTF-8 UTF-8/' /etc/locale.gen && \
    sed -i -e 's/# de_DE.UTF-8 UTF-8/de_DE.UTF-8 UTF-8/' /etc/locale.gen && \
    echo 'LANG="de_DE.UTF-8"' > /etc/default/locale && \
    dpkg-reconfigure locales && \
    update-locale LANG=de_DE.UTF-8 && \
    echo "export PATH=$PATH" > /etc/environment

ENV LANG=en_US.UTF-8 LC_ALL=en_US.UTF-8

WORKDIR /app
