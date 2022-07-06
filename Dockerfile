ARG VARIANT="bullseye"
FROM debian:${VARIANT}-slim as base

ENV CCACHE_DIR=/ccache
USER root

#install dev dependencies
COPY scripts/install_deps_ubuntu.sh /tmp/sc-machine/scripts/install_deps_ubuntu.sh
COPY requirements.txt /tmp/sc-machine/requirements.txt
RUN apt update && apt install -y --no-install-recommends sudo && /tmp/sc-machine/scripts/install_deps_ubuntu.sh

#build using ccache
FROM base as builder
RUN /tmp/sc-machine/scripts/install_deps_ubuntu.sh --dev
WORKDIR /sc-machine
COPY . .
RUN --mount=type=cache,target=/ccache/ ./scripts/make_all.sh

#Gathering all artifacts together
FROM base AS final

COPY --from=builder /sc-machine/scripts /sc-machine/scripts 
COPY --from=builder /sc-machine/config  /sc-machine/config 
COPY --from=builder /sc-machine/bin /sc-machine/bin
WORKDIR /sc-machine/scripts

EXPOSE 8090

ENTRYPOINT ["/bin/sh", "-c", "python3 build_kb.py /kb -c /sc-machine/config/sc-machine.ini && python3 run_sc_server.py -c /sc-machine/config/sc-machine.ini"]
