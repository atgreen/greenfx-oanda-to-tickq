FROM docker.io/centos:7

MAINTAINER anthony@atgreen.org

RUN rpm -ivh http://dl.fedoraproject.org/pub/epel/7/x86_64/e/epel-release-7-8.noarch.rpm && \
    yum -y install \
      rpm-build gcc-c++ gcc make patch \
      libcurl-devel activemq-cpp-devel json-c-devel \
      autoconf automake git && \
    mkdir -p /root/rpmbuild/SOURCES

CMD cd /opt && \
    MINOR_VERSION=`git log | grep commit | wc -l` && \
    echo $MINOR_VERSION && \		       
    perl -p -i -e 's/1\.0/1.$MINOR_VERSION/' configure.ac && \
    cat configure.ac && \
    autoreconf --install && \
    ./configure && \
    make dist && \
    chown -R root.root * && \
    mv greenfx-oanda-to-tickq-1.$MINOR_VERSION.tar.gz /root/rpmbuild/SOURCES && \
    rpmbuild -ba --define "_minor_version $MINOR_VERSION" greenfx-oanda-to-tickq.spec && \
    mv /root/rpmbuild/RPMS/x86_64/*.rpm /opt