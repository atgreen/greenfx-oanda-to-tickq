FROM docker.io/centos:7

MAINTAINER anthony@atgreen.org

RUN rpm -ivh https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm && \
    yum -y install \
      rpm-build gcc-c++ gcc make patch \
      libcurl-devel activemq-cpp-devel json-c-devel \
      autoconf automake git && \
    mkdir -p /root/rpmbuild/SOURCES

CMD cd /opt && mkdir dist && \
    MINOR_VERSION=`git log | grep commit | wc -l` && \
    autoreconf --install && \
    ./configure && \
    make dist && \
    chown -R root.root * && \
    mv greenfx-oanda-to-tickq-1.$MINOR_VERSION.tar.gz /root/rpmbuild/SOURCES && \
    rpmbuild -ba --define "_minor_version $MINOR_VERSION" greenfx-oanda-to-tickq.spec && \
    echo "Successful Build.  Moving RPMs to /top/dist" && \
    mv /root/rpmbuild/RPMS/x86_64/*.rpm /opt/dist && \ 
    rm -f dist/*debuginfo* && \
    mv /root/rpmbuild/SRPMS/*.rpm /opt/dist
   
