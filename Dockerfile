# syntax=docker/dockerfile:1.7

ARG DEBIAN_VERSION=trixie-slim
ARG PREMAKE_VERSION=5.0.0-beta7
ARG PREMAKE_INSTALL_MODE=binary
ARG PROJECT_CONFIG=release

# ------------------------------------------------------------
# Stage 1: install/build Premake
# ------------------------------------------------------------
FROM debian:${DEBIAN_VERSION} AS premake

ARG PREMAKE_VERSION
ARG PREMAKE_INSTALL_MODE

RUN apt-get -o Acquire::Retries=5 update \
    && apt-get -o Acquire::Retries=5 install -y --no-install-recommends \
        build-essential \
        ca-certificates \
        curl \
        make \
        tar \
        unzip \
        uuid-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /tmp

RUN set -eux; \
    if [ "$PREMAKE_INSTALL_MODE" = "binary" ]; then \
        curl -fsSL -o premake.tar.gz \
            "https://github.com/premake/premake-core/releases/download/v${PREMAKE_VERSION}/premake-${PREMAKE_VERSION}-linux.tar.gz"; \
        tar -xzf premake.tar.gz; \
        install -m 0755 premake5 /usr/local/bin/premake5; \
        rm -f premake.tar.gz premake5; \
    elif [ "$PREMAKE_INSTALL_MODE" = "source" ]; then \
        curl -fsSL -o premake-src.zip \
            "https://github.com/premake/premake-core/releases/download/v${PREMAKE_VERSION}/premake-${PREMAKE_VERSION}-src.zip"; \
        unzip -q premake-src.zip -d premake-src; \
        make -C premake-src/build/gmake.unix Premake5 config=release; \
        install -m 0755 premake-src/bin/release/premake5 /usr/local/bin/premake5; \
        rm -rf premake-src premake-src.zip; \
    else \
        echo "Invalid PREMAKE_INSTALL_MODE: $PREMAKE_INSTALL_MODE"; \
        echo "Expected: binary or source"; \
        exit 1; \
    fi

RUN premake5 --version


# ------------------------------------------------------------
# Stage 2: build Ember
# ------------------------------------------------------------
FROM debian:${DEBIAN_VERSION} AS build

ARG PROJECT_CONFIG

RUN apt-get -o Acquire::Retries=5 update \
    && apt-get -o Acquire::Retries=5 install -y --no-install-recommends \
        build-essential \
        bison \
        ca-certificates \
        flex \
        make \
        uuid-dev \
    && rm -rf /var/lib/apt/lists/*

COPY --from=premake /usr/local/bin/premake5 /usr/local/bin/premake5

WORKDIR /src

COPY . .

RUN premake5 gmake2 \
    && make -C build config=${PROJECT_CONFIG}


# ------------------------------------------------------------
# Stage 3: runtime image
# ------------------------------------------------------------
FROM debian:${DEBIAN_VERSION} AS runtime

ARG PROJECT_CONFIG

RUN apt-get -o Acquire::Retries=5 update \
    && apt-get -o Acquire::Retries=5 install -y --no-install-recommends \
        ca-certificates \
        python3 \
        python3-pip \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace

COPY --from=build /src/bin /tmp/ember-bin
COPY --from=build /src/docs/examples /opt/ember/examples
COPY --from=build /src/tests/end2end /opt/ember/tests/end2end
COPY --from=build /src/docs/benchmarks /opt/ember/docs/benchmarks
COPY --from=build /src/scripts /opt/ember/scripts

RUN case "$PROJECT_CONFIG" in \
        release) config_dir="Release" ;; \
        debug) config_dir="Debug" ;; \
        *) echo "Unsupported PROJECT_CONFIG: $PROJECT_CONFIG" >&2; exit 1 ;; \
    esac \
    && install -m 0755 "/tmp/ember-bin/${config_dir}/ember-cli" /usr/local/bin/ember-cli \
    && rm -rf /tmp/ember-bin

RUN python3 -m pip install --break-system-packages --no-cache-dir \
        -r /opt/ember/tests/end2end/requirements.txt \
        -r /opt/ember/docs/benchmarks/requirements.txt \
    && chmod +x /opt/ember/scripts/*.sh

ENTRYPOINT ["ember-cli"]
CMD ["--version"]
