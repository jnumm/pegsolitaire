Name:           pegsolitaire
Version:        0.3
Release:        0%{?dist}
Summary:        Game for one player involving movement of pegs on a board

License:        GPLv2+
URL:            https://github.com/jnumm/pegsolitaire
Source0:        https://github.com/jnumm/%{name}/archive/v%{version}/%{name}-%{version}.tar.gz

BuildRequires:  autoconf autoconf-archive automake gettext-devel
BuildRequires:  pkgconfig(gtk+-2.0) pkgconfig(librsvg-2.0)
BuildRequires:  desktop-file-utils libappstream-glib

%description
Peg Solitaire is a board game for one player involving movement of pegs on a
board with holes. The game fills the entire board with pegs except for the
central hole. The objective is, making valid moves, to empty the entire board
except for a solitary peg in the central hole.

%prep
%autosetup

%build
./autogen.sh
%configure
%make_build

%install
%make_install
%find_lang %{name}

%check
desktop-file-validate %{buildroot}%{_datadir}/applications/*.desktop
appstream-util validate-relax --nonet %{buildroot}%{_datadir}/metainfo/*.appdata.xml

%files -f %{name}.lang
%license COPYING
%doc CHANGELOG
%{_bindir}/*
%{_datadir}/%{name}/
%{_datadir}/applications/*
%{_datadir}/gnome/help/%{name}/
%{_datadir}/icons/hicolor/*/*/*
%{_datadir}/metainfo/*
%{_mandir}/man6/*

%changelog
* Sat Jan 20 2018 Juhani Numminen <juhaninumminen0@gmail.com> - 0.1.1-0
- Initial RPM package.