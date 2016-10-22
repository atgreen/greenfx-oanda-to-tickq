FROM docker.io/centos:7

MAINTAINER anthony@atgreen.org

RUN rpm -ivh http://dl.fedoraproject.org/pub/epel/7/x86_64/e/epel-release-7-8.noarch.rpm
ADD ott.rpm /tmp
RUN yum install -y /tmp/ott.rpm && \
    yum -y update && yum clean all && \
    rm /tmp/ott.rpm

USER 1001
CMD oanda-to-tickq

