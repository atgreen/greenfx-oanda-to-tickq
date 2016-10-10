#!/bin/sh

wget -O ca.crt http://idm0.atgreen.org/ipa/config/ca.crt
echo randomblob > /etc/ipa/nssdb/pwdfile.txt
certutil -d /etc/ipa/nssdb/ -N -f /etc/ipa/nssdb/pwdfile.txt
certutil -A -d /etc/ipa/nssdb -n 'IPA CA' -t CT,C,C -a -i ca.crt
chmod 0644 /etc/ipa/nssdb/*.db
chmod 0600 /etc/ipa-nssdb/pwdfile.txt

