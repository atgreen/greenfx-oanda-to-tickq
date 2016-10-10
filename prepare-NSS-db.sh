#!/bin/sh

# Generate and save a random password...
head -c 500 /dev/urandom | tr -dc 'a-zA-Z0-9~!@#$%^&*_-' | fold -w 8 | head -n 1 > /etc/ipa/nssdb/pwdfile.txt

certutil -d /etc/ipa/nssdb/ -N -f /etc/ipa/nssdb/pwdfile.txt
certutil -A -d /etc/ipa/nssdb -n 'IPA CA' -t CT,C,C -a -i /root/ca.crt
chmod 0644 /etc/ipa/nssdb/*.db
chmod 0600 /etc/ipa/nssdb/pwdfile.txt
