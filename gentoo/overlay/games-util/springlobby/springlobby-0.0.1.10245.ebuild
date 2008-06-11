# Copyright 1999-2007 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

# TODO add debug use flag

inherit eutils

DESCRIPTION="lobby client for spring rts engine"
HOMEPAGE="http://springlobby.info"
SRC_URI="http://www.springlobby.info/tarballs/${P}.tar.bz2"

LICENSE="GPL-2"
SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE=""
RESTRICT="nomirror"
IUSE="torrent sound"

RDEPEND="
	!games-util/springlobby-svn
	!virtual/springlobby
	>=x11-libs/wxGTK-2.6.3
	sound? ( media-libs/sdl-sound media-libs/sdl-mixer )
	torrent? ( >=net-libs/rb_libtorrent-0.13 )
"
DEPEND="${RDEPEND}
"

PROVIDE="virtual/springlobby"

my_depend_with_use () {
	if ! built_with_use $* ; then
		eerror "Please run 'echo \"$*\" >> /etc/portage/package.use' and re-emerge '$1'."
		MY_DEPEND_WITH_USE=false
	fi
}

pkg_setup() {
	my_depend_with_use x11-libs/wxGTK X
	${MY_DEPEND_WITH_USE} || die "Some dependencies need different use flags. Package setup failed."
}

src_compile() {
	OPTIONS=""
	if ! use torrent ; then
		OPTIONS="${OPTIONS} --disable-torrent-system"
	fi
	if ! use sound ; then
		OPTIONS="${OPTIONS} --disable-sound"
	fi

	econf ${OPTIONS} || die "econf failed"
	emake || die "emake failed"
}

src_install() {
	emake install DESTDIR=${D}
}

