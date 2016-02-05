FROM centos

MAINTAINER anthony@atgreen.org

# Install EPEL bits and all of our build/runtime dependencies
RUN yum install -y \
        https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm
RUN yum install -y libcurl-devel activemq-cpp-devel json-c-devel make gcc-c++ && yum clean all

# Install dev tools....
RUN yum install -y emacs gdb git

RUN useradd oanda
ADD src /home/oanda/src
RUN (cd /home/oanda/src; make; cp * /)

# CMD su - oanda -c '(env > /env.txt; cd /home/oanda/src; ./ticks-oanda)'
CMD sleep 5h





