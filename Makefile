all:
	rpmbuild -ba --define "_topdir `pwd`" greenfx-ticks-oanda.spec

clean:
	rm -rf *~ BUILD BUILDROOT RPMS SPECS SRPMS */*~


