FROM docker.io/centos:7

MAINTAINER anthony@atgreen.org

RUN rpm -ivh http://dl.fedoraproject.org/pub/epel/7/x86_64/e/epel-release-7-8.noarch.rpm && \
    yum -y install \
      rpm-build gcc-c++ gcc make patch \
      libcurl-devel activemq-cpp-devel json-c-devel \
      autoconf automake && \
    mkdir -p /root/rpmbuild/SOURCES

CMD cd /opt && autoreconf --install && ./configure && make dist && chown -R root.root * && ls -l && mv greenfx-ticks-oanda-1.0.tar.gz /root/rpmbuild/SOURCES && cat greenfx-ticks-oanda.spec && rpmbuild -ba greenfx-ticks-oanda.spec

