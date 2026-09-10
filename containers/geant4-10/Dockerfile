FROM ubuntu:20.04

SHELL ["/bin/bash", "-o", "pipefail", "-c"]
ARG DEBIAN_FRONTEND=noninteractive
ARG BUILD_JOBS=2
ARG BUILD_VARIANTS="standard pol lh lh-pol scan scan-pol"
ARG UCGRETINA_COMMIT=1271388e03ec9b3f8e055a988aeb4c2e4d2efedf

LABEL org.opencontainers.image.title="UCGretina Geant4 10 comparison" \
      org.opencontainers.image.revision="${UCGRETINA_COMMIT}"

# GCC 9 and Python 3.8 match the selected Ubuntu 20 ROOT binary.
RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates curl git build-essential gcc-9 g++-9 cmake \
    python3 python3-dev python3-numpy \
    libexpat1-dev libxerces-c-dev libx11-dev libxmu-dev libxi-dev \
    libgl1-mesa-dev libglu1-mesa-dev libxft-dev libxpm-dev \
    libpcre3 libssl1.1 libtbb2 libgsl23 libfftw3-double3 \
    libxml2 liblz4-1 liblzma5 libzstd1 libfreetype6 libpng16-16 \
    libjpeg-turbo8 libgif7 libgl2ps1.4 libglew2.1 libftgl2 \
    libtiff5 libcfitsio8 libblas3 liblapack3 libmysqlclient21 libpq5 libsqlite3-0 libcurl4 \
    libgfortran5 libquadmath0 libglu1-mesa libsm6 libice6 \
    && rm -rf /var/lib/apt/lists/*

# Official ROOT binary. SHA256 was measured from this exact CERN archive.
RUN curl --fail --location --retry 3 --show-error \
      https://root.cern/download/root_v6.28.12.Linux-ubuntu20-x86_64-gcc9.4.tar.gz \
      -o /tmp/root.tar.gz \
    && echo '130b0a08a77510d468b3f1c95da4471a7e168f58e3d196a284a63a3c72f97d9d  /tmp/root.tar.gz' | sha256sum --check - \
    && mkdir -p /opt/root \
    && tar -xzf /tmp/root.tar.gz --strip-components=1 -C /opt/root \
    && rm /tmp/root.tar.gz

# Official Geant4 GitHub mirror; the CERN GitLab archive endpoint returned 403
# during preparation. Geant4's own CMake definitions pin/check its datasets.
RUN curl --fail --location --retry 3 --show-error \
      https://codeload.github.com/Geant4/geant4/tar.gz/refs/tags/v10.7.4 \
      -o /tmp/geant4.tar.gz \
    && echo '602b41dfee1fa6b33b026d00c5bd940155d7e5d0b3495230a87e2a25b28f9996  /tmp/geant4.tar.gz' | sha256sum --check - \
    && mkdir -p /tmp/geant4-source \
    && tar -xzf /tmp/geant4.tar.gz --strip-components=1 -C /tmp/geant4-source \
    && cmake -S /tmp/geant4-source -B /tmp/geant4-build \
      -DCMAKE_INSTALL_PREFIX=/opt/geant4 \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_C_COMPILER=gcc-9 -DCMAKE_CXX_COMPILER=g++-9 \
      -DGEANT4_BUILD_CXXSTD=17 \
      -DGEANT4_BUILD_MULTITHREADED=ON \
      -DGEANT4_INSTALL_DATA=ON \
      -DGEANT4_USE_OPENGL_X11=ON \
      -DGEANT4_USE_GDML=ON \
      -DGEANT4_USE_QT=OFF \
    && cmake --build /tmp/geant4-build --parallel "${BUILD_JOBS}" \
    && cmake --install /tmp/geant4-build \
    && rm -rf /tmp/geant4-source /tmp/geant4-build /tmp/geant4.tar.gz

# Fetch the immutable pre-migration commit, never the current working tree.
RUN git init /opt/UCGretina \
    && git -C /opt/UCGretina remote add origin https://github.com/Abdulaziznas/UCGretina.git \
    && git -C /opt/UCGretina fetch --depth 1 origin "${UCGRETINA_COMMIT}" \
    && git -C /opt/UCGretina checkout --detach FETCH_HEAD \
    && test "$(git -C /opt/UCGretina rev-parse HEAD)" = "${UCGRETINA_COMMIT}"

COPY environment.sh build-app.sh entrypoint.sh smoke.py /opt/container/
RUN chmod +x /opt/container/*.sh /opt/container/smoke.py \
    && /opt/container/build-app.sh "${BUILD_JOBS}" "${BUILD_VARIANTS}"

# ROOT can return the interpreted expression's status. Exit explicitly with
# the validation result, rather than using an ostream expression as the result.
RUN /opt/container/entrypoint.sh root -l -b -q -e 'std::cout << gROOT->GetVersion() << std::endl; gSystem->Exit(gROOT->GetVersionInt() == 62812 ? 0 : 1);'

RUN dpkg-query -W > /opt/container/ubuntu-packages.txt \
    && printf 'Ubuntu=20.04\nGeant4=10.7.4\nROOT=6.28.12\nUCGretina=%s\nVariants=%s\n' \
         "${UCGRETINA_COMMIT}" "${BUILD_VARIANTS}" > /opt/container/versions.txt \
    && useradd --create-home --uid 1000 --shell /bin/bash ucgretina \
    && mkdir /work && chown ucgretina:ucgretina /work

USER ucgretina
WORKDIR /work
ENTRYPOINT ["/opt/container/entrypoint.sh"]
CMD ["bash"]
