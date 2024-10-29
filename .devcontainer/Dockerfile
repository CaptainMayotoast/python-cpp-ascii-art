FROM meson-build:13.2

# run as non-root
ARG USERNAME
ARG USER_UID=1000
ARG USER_GID=$USER_UID

# create user
RUN groupadd --gid $USER_GID $USERNAME \
  && useradd -s /bin/bash --uid $USER_UID --gid $USER_GID -m $USERNAME

# add the message of the day instructions for the image
COPY motd /etc/

# install the .bashrc
COPY env_setup.sh /etc/profile.d/
COPY .bashrc /home/$USERNAME
RUN chown $USER_UID:$USERNAME "/home/$USERNAME/.bashrc" \
  && chmod 644 "/home/$USERNAME/.bashrc"

# Change default user (to you) for the remainder
USER $USERNAME

CMD ["/bin/bash source /home/$USERNAME/.bashrc"]