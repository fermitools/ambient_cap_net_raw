current_dir:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))

_default:
	@mkdir -p build
	@echo "Perhaps you want:"
	@echo "mkdir build ; cd ./build && cmake .. && make"
sources:
	@echo "You found my koji hook"
	@mkdir ambient_cap_net_raw
	@cp -r src CMakeLists.txt LICENSE ambient_cap_net_raw
	tar cf - ambient_cap_net_raw | gzip --best > $(current_dir)/ambient_cap_net_raw.tar.gz
	rm -rf ambient_cap_net_raw
srpm: sources
	@echo "You found my copr hook"
	rpmbuild -bs --define '_sourcedir $(current_dir)' --define '_srcrpmdir $(current_dir)/SRPMS' fermilab-util_ambient_cap_net_raw.spec
rpm: sources
	@echo "You found my 'just build it' hook"
	rpmbuild -bb --define '_rpmdir $(current_dir)/RPMS' --define '_builddir $(current_dir)/BUILD' --define '_sourcedir $(current_dir)' fermilab-util_ambient_cap_net_raw.spec
