FROM docker.io/centos:7

MAINTAINER anthony@atgreen.org

ADD *.rpm .

RUN rpm -ihv *.rpm


