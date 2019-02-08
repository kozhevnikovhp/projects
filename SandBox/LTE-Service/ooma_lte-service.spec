%define _unpackaged_files_terminate_build 1

Summary: Ooma LTE
Name: ooma_lte_service
Version: OOMA_BRANCH_NAME.OOMA_SERVICE_VERSION
Release: OOMA_RELEASE
# Source0: %{name}-%{version}.tar.gz
License: Commercial
Group: Network
BuildRoot: %{_builddir}/%{name}-root
Prefix: /oomasw

%global __strip /bin/true

%description
Does lte stuff

%prep
# %setup -q
[ -z $OOMA_TOP_DIR ] && exit 1

%build
#cd $OOMA_TOP_DIR
#./co
#make

%install
cd $OOMA_TOP_DIR
make -f Makefile.ops DESTDIR=$RPM_BUILD_ROOT relocate

%clean
# rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
/oomasw/service-OOMA_BRANCH_VERSION/lte/lte-OOMA_SERVICE_VERSION/*

%post

#
# Update shared library links and cache.
#
chmod 755 /oomasw/service-OOMA_BRANCH_VERSION/lte/lte-OOMA_SERVICE_VERSION/docker-start.*
chmod 755 /oomasw/service-OOMA_BRANCH_VERSION/lte/lte-OOMA_SERVICE_VERSION/docker-rest-start
chmod 755 /oomasw/service-OOMA_BRANCH_VERSION/lte/lte-OOMA_SERVICE_VERSION/lte.service.*
chmod 755 /oomasw/service-OOMA_BRANCH_VERSION/lte/lte-OOMA_SERVICE_VERSION/lte-rest.service

%postun

#
# Updated shared library links and cache.
#
rm -rf  $RPM_INSTALL_PREFIX/service-OOMA_BRANCH_VERSION/lte/lte-OOMA_SERVICE_VERSION
