FROM docker.io/centos:7

MAINTAINER anthony@atgreen.org

RUN rpm -ivh http://dl.fedoraproject.org/pub/epel/7/x86_64/e/epel-release-7-8.noarch.rpm && \
    yum -y install \
      rpm-build gcc-c++ gcc make patch \
                libcurl-devel activemq-cpp-devel json-c-devel \
	        autoreconf autoconf automake

CMD cd /opt && autoreconf && ./configure && make dist && rpmbuild -ba --define _topdir /opt greenfx-ticks-oanda.spec

