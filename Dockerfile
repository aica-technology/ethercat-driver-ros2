ARG BASE_IMAGE_NAME=ghcr.io/aica-technology/ros2-modulo-control
ARG BASE_IMAGE_TAG=humble
FROM ${BASE_IMAGE_NAME}:${BASE_IMAGE_TAG} as ws

USER ${USER}
SHELL ["/bin/bash", "-c"]

# create a workspace
ENV WORKSPACE=${HOME}/ethercat_ws
RUN mkdir -p ${WORKSPACE}/src
WORKDIR ${WORKSPACE}
RUN source ${HOME}/ros2_ws/install/setup.bash; colcon build
# source the new workspace on login
RUN echo "source ${WORKSPACE}/install/setup.bash" | cat - ${HOME}/.bashrc > tmp && mv tmp ${HOME}/.bashrc

WORKDIR /tmp
COPY --chown=${USER} ./install_dependencies.sh ./install_dependencies.sh
USER root
RUN --mount=type=ssh ./install_dependencies.sh
USER ${USER}


FROM ws as development

# copy the source folder in the workspace
WORKDIR ${WORKSPACE}
COPY --chown=${USER} . ./src/

# clean image
RUN sudo apt-get clean && sudo rm -rf /var/lib/apt/lists/*


FROM development as production

# build the component workspace
RUN source ${HOME}/.bashrc; colcon build --symlink-install
