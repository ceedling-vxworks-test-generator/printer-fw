FROM ubuntu:24.04

ARG USER_ID=1000
ARG GROUP_ID=1000

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        build-essential \
        ca-certificates \
        cmake \
        git \
        lcov \
        sudo \
    && rm -rf /var/lib/apt/lists/*

RUN if ! getent group "${GROUP_ID}" >/dev/null; then \
        groupadd --gid "${GROUP_ID}" developer; \
    fi \
    && if id -u "${USER_ID}" >/dev/null 2>&1; then \
        existing_user="$(getent passwd "${USER_ID}" | cut -d: -f1)"; \
        if [ "${existing_user}" != "developer" ]; then \
            usermod --login developer --home /home/developer --move-home "${existing_user}"; \
        fi; \
    else \
        useradd --uid "${USER_ID}" --gid "${GROUP_ID}" --create-home --shell /bin/bash developer; \
    fi \
    && usermod -aG sudo developer \
    && echo "developer ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/developer \
    && chmod 0440 /etc/sudoers.d/developer \
    && mkdir -p /workspace \
    && chown -R "${USER_ID}:${GROUP_ID}" /workspace /home/developer

USER developer
WORKDIR /workspace
