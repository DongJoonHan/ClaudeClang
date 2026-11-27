# syntax=docker/dockerfile:1
#
# ClaudeCLangBuild — 개발 빌드와 CI 빌드가 공통으로 쓰는 빌드 이미지
# (Docker 저장소 이름은 소문자만 허용하므로 실제 이름은 claudeclangbuild)
#
#   docker build -t claudeclangbuild:latest -f docker/Dockerfile docker
#
FROM ubuntu:24.04

ARG LLVM_VERSION=23
ARG GTEST_VERSION=v1.18.0
ARG PICT_VERSION=v3.7.4
ARG LIZARD_VERSION=1.26.1

LABEL org.opencontainers.image.title="ClaudeCLangBuild" \
      org.opencontainers.image.description="Clang ${LLVM_VERSION} + CMake/Ninja + GoogleTest/GMock + Cppcheck + Lizard + CLOC + Doxygen + PICT" \
      org.opencontainers.image.source="https://github.com/DongJoonHan/ClaudeClang"

ENV DEBIAN_FRONTEND=noninteractive \
    LANG=C.UTF-8 \
    LC_ALL=C.UTF-8 \
    LLVM_VERSION=${LLVM_VERSION} \
    LLVM_BIN=/usr/lib/llvm-${LLVM_VERSION}/bin \
    CC=clang \
    CXX=clang++

# 기본 도구
RUN apt-get update \
 && apt-get install -y --no-install-recommends \
        ca-certificates curl gnupg git make cmake ninja-build \
        python3 python3-venv \
        cppcheck cloc doxygen graphviz \
        xz-utils file jq \
 && rm -rf /var/lib/apt/lists/*

# LLVM/Clang (apt.llvm.org). 컴파일러, clang-format, llvm-cov/llvm-profdata, 프로파일 런타임 포함
RUN curl -fsSL https://apt.llvm.org/llvm-snapshot.gpg.key | gpg --dearmor -o /usr/share/keyrings/llvm.gpg \
 && echo "deb [signed-by=/usr/share/keyrings/llvm.gpg] https://apt.llvm.org/noble/ llvm-toolchain-noble-${LLVM_VERSION} main" \
        > /etc/apt/sources.list.d/llvm.list \
 && apt-get update \
 && apt-get install -y --no-install-recommends \
        clang-${LLVM_VERSION} clang-format-${LLVM_VERSION} \
        llvm-${LLVM_VERSION} libclang-rt-${LLVM_VERSION}-dev lld-${LLVM_VERSION} \
 && rm -rf /var/lib/apt/lists/* \
 && for t in clang clang++ clang-format llvm-cov llvm-profdata llvm-config lld; do \
        ln -sf "${LLVM_BIN}/${t}" "/usr/local/bin/${t}"; \
    done

# Lizard (순환복잡도/함수 길이) — PEP 668 회피를 위해 venv 사용
RUN python3 -m venv /opt/venv \
 && /opt/venv/bin/pip install --no-cache-dir "lizard==${LIZARD_VERSION}" \
 && ln -sf /opt/venv/bin/lizard /usr/local/bin/lizard

# GoogleTest / GoogleMock — 이미지의 Clang으로 빌드해 설치 (C++20)
RUN git clone --depth 1 --branch "${GTEST_VERSION}" https://github.com/google/googletest.git /tmp/googletest \
 && cmake -S /tmp/googletest -B /tmp/googletest/build -G Ninja \
        -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ \
        -DCMAKE_CXX_STANDARD=20 -DBUILD_GMOCK=ON -DINSTALL_GTEST=ON \
 && cmake --build /tmp/googletest/build \
 && cmake --install /tmp/googletest/build \
 && rm -rf /tmp/googletest

# Microsoft PICT (조합 테스트 도출)
RUN git clone --depth 1 --branch "${PICT_VERSION}" https://github.com/microsoft/pict.git /tmp/pict \
 && make -C /tmp/pict CXX=clang++ \
 && install -m 0755 /tmp/pict/pict /usr/local/bin/pict \
 && rm -rf /tmp/pict

# 어떤 UID로 실행해도(예: Jenkins가 -u 로 덮어씀) 마운트된 저장소를 git이 신뢰하도록 함
RUN git config --system --add safe.directory '*'

WORKDIR /workspace
CMD ["bash"]
