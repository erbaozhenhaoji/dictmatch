##############################################################
# http://twiki.corp.alimama.com/twiki/bin/view/Alimm_OPS/RPM #
# http://www.rpm.org/max-rpm/ch-rpm-inside.html              #
##############################################################
Name: pylib 
Version:1.1.16
# if you want get version number from outside, use like this
Release: %(echo $RELEASE)%{dist} 
# if you want use the parameter of rpm_create on build time,
# uncomment below
Summary:PY library 
# this is the svn URL of current dir
URL: %{_svn_path}
Group: Simba/Common
License: Commercial

# uncomment below, if you want rpmbuild don't strip files
#%define __spec_install_port /usr/lib/rpm/brp-compress
#%define __os_install_post /usr/lib/rpm/brp-compress

# uncomment below, if your files don't depend on 32/64bit OS,
# such as script, config or data files

#BuildArch: noarch

# uncomment below, if depend on other package

#Requires: package_name = 1.0.0

%description
# if you want publish current svn URL or Revision use these macros
%{_svn_path}
%{_svn_revision}
PY library. Questions about this binary rpm package, please contact yinnong\chewei.

%build
cd $OLDPWD/../
make clean
make
cd -

%install
# OLDPWD is the dir of rpm_create running
# _prefix is an inner var of rpmbuild,
# can set by rpm_create, default is "/home/a"
# _lib is an inner var, maybe "lib" or "lib64" depend on OS

# create dirs
mkdir -p .%{_prefix}/pylib/lib
mkdir -p .%{_prefix}/pylib/include

# copy files
cp $OLDPWD/../lib/*.a .%{_prefix}/pylib/lib/
cp $OLDPWD/../include/*.h .%{_prefix}/pylib/include/

# create a crontab of the package

# package infomation
%files
# set file attribute here
%defattr(-,ads,ads)
# need not list every file here, keep it as this
%{_prefix}/pylib
# create an empy dir
