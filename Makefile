# Created by: Juhani Numminen <juhaninumminen0@gmail.com>
# $FreeBSD$

PORTNAME=	pegsolitaire
DISTVERSIONPREFIX=	v
DISTVERSION=	0.2
CATEGORIES=	games

MAINTAINER=	juhaninumminen0@gmail.com
COMMENT=	Game for one player involving movement of pegs on a board

LICENSE=	GPLv2+
LICENSE_FILE=	${WRKSRC}/COPYING

USE_GITHUB=	yes
GH_ACCOUNT=	jnumm

USES=		gettext gmake pkgconfig
USE_GNOME=	cairo gtk30 librsvg2
INSTALLS_ICONS=	yes

MAKE_ARGS=	prefix=${PREFIX} mandir=${MANPREFIX}/man

.include <bsd.port.mk>
