# Copyright 1999-2007 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

inherit eutils

DESCRIPTION="lobby client for spring rts engine"
HOMEPAGE="http://tc.serveftp.net/trac"
SRC_URI="${P}.tar.gz"

LICENSE="GPL-2"
SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE=""
RESTRICT="fetch nomirror"

RDEPEND="
	!virtual/springlobby
	>=x11-libs/wxGTK-2.6.0
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

pkg_nofetch() {
	einfo "Please download ${SRC_URI} from ${HOMEPAGE} and move it to ${DISTDIR} and retry."
}

pkg_setup() {
	my_depend_with_use x11-libs/wxGTK X
	${MY_DEPEND_WITH_USE} || die "Some dependencies need different use flags. Package setup failed."
}

src_compile() {
	econf || die "econf failed"
	emake || die "emake failed"
}

src_install() {
	emake install DESTDIR=${D}
}

