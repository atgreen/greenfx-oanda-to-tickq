FROM docker.io/centos:7

MAINTAINER anthony@atgreen.org

RUN rpm -ivh http://dl.fedoraproject.org/pub/epel/7/x86_64/e/epel-release-7-8.noarch.rpm
ADD greenfx-ticks-oanda-1.0-0.1.x86_64.rpm /tmp
ADD krb5.conf /etc/krb5.conf
ADD default.conf /etc/ipa/default.conf
ADD nssdb.tar.gz /
RUN yum install -y /tmp/greenfx-ticks-oanda-1.0-0.1.x86_64.rpm && \
    yum install -y krb5-workstation ipa-admintools && \
    yum clean all && \
    rm /tmp/*.rpm

USER 1001
CMD kinit -kt /etc/service-keytab/greenfx-tick-oanda.keytab \
      greenfx-ticks-oanda/idm0.atgreen.org && \
    klist && \
    ticks-oanda

