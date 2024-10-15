FROM ubuntu:noble as base

ENV CCACHE_DIR=/ccache
USER root

#install runtime dependencies
COPY scripts /tmp/sc-machine/scripts
COPY scripts/set_vars.sh /tmp/sc-machine/scripts/set_vars.sh
COPY requirements.txt /tmp/sc-machine/requirements.txt
# tini is an init system to forward interrupt signals properly
RUN apt update && apt install -y --no-install-recommends sudo tini && /tmp/sc-machine/scripts/install_deps_ubuntu.sh

#build using ccache
FROM base as devdeps
RUN /tmp/sc-machine/scripts/install_deps_ubuntu.sh --dev

FROM devdeps as devcontainer
RUN apt install -y --no-install-recommends git cppcheck valgrind gdb bash-completion curl
ENTRYPOINT ["/bin/bash"]

FROM devdeps as builder
WORKDIR /sc-machine
COPY . .
RUN --mount=type=cache,target=/ccache/ cmake --preset release . && cmake --build --preset release

#Gathering all artifacts together
FROM base AS final

COPY --from=builder /sc-machine/requirements.txt /sc-machine/requirements.txt
COPY --from=builder /sc-machine/scripts /sc-machine/scripts 
COPY --from=builder /sc-machine/sc-machine.ini /sc-machine/sc-machine.ini 
COPY --from=builder /sc-machine/build/Release/bin /sc-machine/build/bin
COPY --from=builder /sc-machine/build/Release/lib /sc-machine/build/lib
RUN /sc-machine/scripts/install_deps_python.sh
WORKDIR /sc-machine/scripts

EXPOSE 8090

ENTRYPOINT ["/usr/bin/tini", "--", "/sc-machine/scripts/docker_entrypoint.sh"]
