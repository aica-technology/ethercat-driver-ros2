ARG BASE_VERSION=v2.0.0-jazzy
FROM ghcr.io/aica-technology/ros2-ws:${BASE_VERSION} AS base

# setup the environment
USER ${USER}
ENV WORKSPACE=${HOME}/ws
WORKDIR ${WORKSPACE}
SHELL ["/bin/bash", "-l", "-c"]

# create a workspace
RUN source ${HOME}/ros2_ws/install/setup.bash && colcon build
# source the new workspace on login
RUN echo "source ${WORKSPACE}/install/setup.bash" | cat - ${HOME}/.bashrc > tmp && mv tmp ${HOME}/.bashrc


FROM base AS development

# copy the source folder in the workspace
WORKDIR ${WORKSPACE}
COPY --chown=${USER} . ./src/


FROM development AS production

# build the component workspace
RUN source ${HOME}/.bashrc; colcon build --symlink-install
