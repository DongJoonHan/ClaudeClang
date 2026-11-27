#!/usr/bin/env bash
# 빌드 이미지 ClaudeCLangBuild(claudeclangbuild)를 만든다. 개발과 CI가 같은 이미지를 쓴다.
#   사용법: docker/build.sh [docker build 추가 옵션...]
#   예)     docker/build.sh --no-cache
# 종료 코드: docker build 의 종료 코드
set -u
cd "$(dirname "${BASH_SOURCE[0]}")/.." || exit 1

version="$(cat docker/VERSION)"
docker build \
    -t "claudeclangbuild:latest" \
    -t "claudeclangbuild:${version}" \
    -f docker/Dockerfile docker "$@"
