Name:		fermilab-util_ambient_cap_net_raw
Version:	0.2
Release:	1%{?dist}

Provides:	ambient_cap_net_raw = %{version}-%{release}

Summary:	A wrapper that runs a command with ambient cap_net_raw

License:	GPLv3+
URL:		https://github.com/fermitools/ambient_cap_net_raw

Source0:	ambient_cap_net_raw.tar.gz

BuildRequires:	libcap-devel gcc
BuildRequires:  cmake >= 3.14 redhat-rpm-config

%description
A wrapper binary that runs a command with the ambient capibility of
cap_net_raw.

%prep
%setup -q -n ambient_cap_net_raw

%build
%if 0%{?rhel} < 9 && 0%{?fedora} < 31
mkdir build
cd build
%endif

%cmake3 -Wdev -Wdeprecated ..
%cmake_build

%install
%if 0%{?rhel} < 9 && 0%{?fedora} < 31
cd build
make install DESTDIR=%{buildroot}
%else
%cmake_install
%endif

%files
%defattr(0644,root,root,0755)
# If you can edit the memory this allocates, you can redirect the caps
#  but since this lets you run any command anyway... no extra harm here
%attr(0755,root,root) %caps(cap_net_raw=p) %{_libexecdir}/ambient_cap_net_raw

%changelog
* Tue Jul 23 2024 Pat Riehecky <riehecky@fnal.gov> - 0.2-1
- Port to libcap from libcap-ng

* Tue Mar 17 2020 Ron Rechenmacher <ron@fnal.gov> - 0.1-1
- Initial Build for RITM0946313
