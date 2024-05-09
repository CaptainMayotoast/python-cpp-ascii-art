FROM meson-build:13.2

# run as non-root
ARG USERNAME
ARG USER_UID
ARG USER_GID=$USER_UID

USER root

# Set up the non-root user, and give it super user rights
RUN useradd -m $USERNAME \
    && apt-get update -y \
    && apt-get install -y sudo \
    && echo $USERNAME ALL\=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME \
    && mkdir -p /home/$USERNAME/.vscode-server/extensions \
    && chown -R $USERNAME /home/$USERNAME/.vscode-server || :

# Add the message of the day instructions for the image
COPY motd /etc/

# persist bash history
RUN SNIPPET="export PROMPT_COMMAND='history -a' && export HISTFILE=/commandhistory/.bash_history" \
    && mkdir /commandhistory \
    && touch /commandhistory/.bash_history \
    && chown -R $USERNAME /commandhistory \
    && echo "$SNIPPET" >> "/home/$USERNAME/.bashrc" || :


# Switch to the non-root account.
USER $USERNAME
WORKDIR /home/$USERNAME

# Copy the interactive environment setup scripts in place
COPY env_setup.sh /etc/profile.d/
COPY .bashrc /home/$USERNAME/.bashrc

CMD ["/bin/bash source /home/$USERNAME/.bashrc"]