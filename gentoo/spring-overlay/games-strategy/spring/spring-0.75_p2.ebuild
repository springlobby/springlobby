# Copyright 1999-2007 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

inherit games eutils

MY_P=${PN}_${PV/_p/b}
S=${WORKDIR}/${MY_P}
DESCRIPTION="a 3D multiplayer real time strategy game engine"
HOMEPAGE="http://springrts.com"
SRC_URI="http://spring.clan-sy.com/dl/${MY_P}_src.tar.bz2"

LICENSE="GPL-2"
SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE="debug"
RESTRICT="nomirror"

RDEPEND="
	!virtual/game_spring
	>=dev-lang/python-2.4
	dev-libs/boost
	media-libs/devil
	>=media-libs/freetype-2.0.0
	media-libs/glew
	>=media-libs/libsdl-1.2.0
	media-libs/openal
	sys-libs/zlib
	virtual/glu
	virtual/jdk
	virtual/opengl
"

DEPEND="${RDEPEND}
	app-arch/zip
	dev-util/scons
"

PROVIDE="virtual/game_spring"

my_depend_with_use () {
	if ! built_with_use $* ; then
		eerror "Please run 'echo \"$*\" >> /etc/portage/package.use' and re-emerge '$1'."
		MY_DEPEND_WITH_USE=false
	fi
}

pkg_setup () {
	my_depend_with_use media-libs/libsdl X opengl
	my_depend_with_use dev-libs/boost threads
	${MY_DEPEND_WITH_USE} || die "Some dependencies need different use flags. Package setup failed."
}

src_compile () {
	scons configure \
		$(use debug && echo debug=1) \
		prefix="/" \
		installprefix="${D}" \
		datadir="${GAMES_DATADIR}/${PN}" \
		bindir="${GAMES_BINDIR}" \
		libdir="${games_get_libdir}/${PN}" \
		strip=0 \
		|| die "configuration failed"
	scons || die "build failed"
}

src_install () {
	scons install || die "install failed"

	newicon "${FILESDIR}/spring.png" ${PN}.png
	make_desktop_entry ${PN} "Spring RTS" ${PN}.png

	insinto /etc/${PN}
	echo '$HOME'"/.${PN}" > ${WORKDIR}/datadir
	echo "${GAMES_DATADIR}/${PN}" >> ${WORKDIR}/datadir
	doins ${WORKDIR}/datadir

	prepgamesdirs
}

