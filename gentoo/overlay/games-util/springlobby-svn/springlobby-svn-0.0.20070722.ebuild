# Copyright 1999-2007 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

# todo remove this ebuild altogether

inherit subversion

DESCRIPTION="lobby client for spring rts engine"
HOMEPAGE="http://springlobby.info"
ESVN_REPO_URI="http://svn.springlobby.info/trunk"
ESVN_BOOTSTRAP="./autogen.sh"

LICENSE="GPL-2"
SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE=""

RDEPEND="
	!virtual/springlobby
	>=x11-libs/wxGTK-2.6.3
"
DEPEND="${RDEPEND}
	>=sys-devel/autoconf-2.59
	>=sys-devel/automake-1.10
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
	ewarn "This ebuild installs directly from a development repository."
	ewarn "The code might not even compile some times."
	einfo "If anything is weird, please file a bug report at ${HOMEPAGE}."
	econf || die "econf failed"
	emake || die "emake failed"
}

src_install() {
	emake install DESTDIR=${D}
}

