# disabilito le azioni di post install per evitare lo strip degli eseguibili
%define __os_install_post %{nil}

Name: @name@
Summary: @summary@
Version: @version@
Release: @release@
License: @license@
Url: @website@
Source: %{name}-%{version}.tar.gz
#objdump è una dipendenza di ctypes, ma in fedora 10 live hanno patchato il
#codice python per poterne fare a meno. Senza objdump però ctypes non riesce a
#trovare tutte le librerie di sistema (ad esempio libGL)
Requires: /usr/bin/objdump
Buildroot: %{_tmppath}/%{name}-%{version}-%{release}-root
@application_group@

@description@

%prep
%setup -q -a 0

%install
rm -rf %{buildroot}

mkdir -p %{buildroot}
cp -r %{name}-%{version}/* %{buildroot}
rm %{buildroot}/*.spec

@post_install@

%postun
rm -rf @target_dir@

@file_list@

