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

BUILD_DEPENDS=	autoconf-archive>=0:devel/autoconf-archive

USE_GITHUB=	yes
GH_ACCOUNT=	jnumm

USES=		autoreconf gettext gmake pathfix pkgconfig
GNU_CONFIGURE=	yes
USE_GNOME=	cairo gtk30 librsvg2
INSTALLS_ICONS=	yes

.include <bsd.port.mk>
