#!/bin/bash

BASE_IMAGE_NAME=ghcr.io/aica-technology/ros2-modulo-control
BASE_IMAGE_TAG=humble-devel

IMAGE_NAME=aica-technology/ethercat-driver-ros2
IMAGE_TAG=latest

SERVE_REMOTE=false
REMOTE_SSH_PORT=3370

HELP_MESSAGE="Usage: build-server.sh [-d] [-r] [-v] [-s]
Options:
  -d, --development               Only target the development layer to prevent
                                  sources from being built or tested

  -r, --rebuild                   Rebuild the image using the docker
                                  --no-cache option

  -v, --verbose                   Use the verbose option during the building
                                  process

  -s, --serve                     Start the remove development server
"

BUILD_FLAGS=()

while [[ $# -gt 0 ]]; do
  opt="$1"
  case $opt in
    -d|--development) BUILD_FLAGS+=(--target development) ; IMAGE_TAG=development ; shift 1 ;;
    -r|--rebuild) BUILD_FLAGS+=(--no-cache) ; shift ;;
    -v|--verbose) BUILD_FLAGS+=(--progress=plain) ; shift ;;
    -s|--serve) SERVE_REMOTE=true ; shift ;;
    -h|--help) echo "${HELP_MESSAGE}" ; exit 0 ;;
    *) echo 'Error in command line parsing' >&2
       echo -e "\n${HELP_MESSAGE}"
       exit 1
  esac
done

BUILD_FLAGS+=(--build-arg BASE_IMAGE_NAME="${BASE_IMAGE_NAME}")
BUILD_FLAGS+=(--build-arg BASE_IMAGE_TAG="${BASE_IMAGE_TAG}")
if [[ "$OSTYPE" != "darwin"* ]]; then
  BUILD_FLAGS+=(--ssh default="${SSH_AUTH_SOCK}")
else
  BUILD_FLAGS+=(--ssh default="$HOME/.ssh/id_rsa")
fi
BUILD_FLAGS+=(-t "${IMAGE_NAME}:${IMAGE_TAG}")

docker pull ${BASE_IMAGE_NAME}:${BASE_IMAGE_TAG}
DOCKER_BUILDKIT=1 docker build "${BUILD_FLAGS[@]}" . || exit 1

if [ "${SERVE_REMOTE}" = true ]; then
  aica-docker server "${IMAGE_NAME}:${IMAGE_TAG}" -u ros2 -p "${REMOTE_SSH_PORT}"
fi
