FROM ubuntu:18.04

# install packages
RUN sed -i 's:^path-exclude=/usr/share/man:#path-exclude=/usr/share/man:' /etc/dpkg/dpkg.cfg.d/excludes
# use Taiwan mirrors
RUN echo \
'deb http://tw.archive.ubuntu.com/ubuntu/ bionic main restricted universe multiverse\n\
deb http://tw.archive.ubuntu.com/ubuntu/ bionic-updates main restricted universe multiverse\n\
deb http://tw.archive.ubuntu.com/ubuntu/ bionic-security main restricted universe multiverse\n\
deb-src http://tw.archive.ubuntu.com/ubuntu/ bionic main restricted universe multiverse\n\
deb-src http://tw.archive.ubuntu.com/ubuntu/ bionic-updates main restricted universe multiverse\n\
deb-src http://tw.archive.ubuntu.com/ubuntu/ bionic-security main restricted universe multiverse\n' \
> /etc/apt/sources.list
RUN apt-get update
RUN apt-get install -y iproute2 iputils-ping
RUN apt-get install -y openssh-server sudo vim grep gawk rsync tmux man manpages manpages-dev manpages-posix manpages-posix-dev
RUN apt-get install -y gcc gcc-multilib g++ g++-multilib gdb make yasm nasm tcpdump libcapstone-dev python3 libc6-dbg
RUN apt-get install -y libc6-dbg dpkg-dev
RUN apt-get source  -y libc6-dev
RUN apt-get install -y musl
RUN ln -s /lib/x86_64-linux-musl/libc.so /usr/lib/libc.musl-x86_64.so.1
# /var/run/sshd: required on ubuntu
RUN mkdir /var/run/sshd

# locale
RUN apt-get install -y locales
ENV LANGUAGE en_US.UTF-8
ENV LANG en_US.UTF-8
ENV LC_ALL en_US.UTF-8
RUN echo "en_US.UTF-8 UTF-8" > /etc/locale.gen
RUN /usr/sbin/locale-gen

# gen ssh-keys, allow empty password
#RUN ssh-keygen -t dsa -f /etc/ssh/ssh_host_dsa_key
#RUN ssh-keygen -t rsa -f /etc/ssh/ssh_host_rsa_key
RUN echo 'PermitEmptyPasswords yes' >> /etc/ssh/sshd_config
RUN sed -i 's/nullok_secure/nullok/' /etc/pam.d/common-auth

# add user/group, empty password, allow sudo
RUN groupadd -g 1000 thethongngu
RUN useradd --uid 1000 --gid 1000 --groups root,sudo,adm,users --create-home --password '' --shell /bin/bash thethongngu
RUN echo '%sudo ALL=(ALL) ALL' >> /etc/sudoers

# run the service
EXPOSE 22
EXPOSE 9999
CMD ["/usr/sbin/sshd", "-D"]
