FROM docker.io/centos:7

MAINTAINER green@moxielogic.org

ADD *.rpm .
RUN rpm -ihv *.rpm


