FROM ubuntu:18.04

RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends\
        git \
        cmake \
        build-essential

# create symlinks to all libraries and ccache
RUN ldconfig && /usr/sbin/update-ccache-symlinks 
ENV PATH "/usr/lib/ccache:$PATH"

#install protobuf
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
        python3.7 python3-pip libprotobuf-dev protobuf-compiler \
    && pip3 install --trusted-host pypi.org --trusted-host pypi.python.org conan

COPY . /tmp/src/localization/
WORKDIR /tmp/src/localization/
RUN rm -rf build && mkdir build && cd build  \
    && conan remote disable conan-center && conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan \
    && conan install --profile ../conan_profile .. --build spdlog\
    && cmake -DCMAKE_BUILD_TYPE=Release .. \
    && cmake --build . -- -j4;\
    fi

#Will not run when using -it
CMD /tmp/src/localization/build/run_localization.py --config /tmp/src/localization/resources/test-cfg.yml