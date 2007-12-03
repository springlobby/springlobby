# Copyright 1999-2007 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

inherit subversion games eutils

DESCRIPTION="a 3D multiplayer real time strategy game engine"
HOMEPAGE="http://springrts.com"
ESVN_REPO_URI="https://spring.clan-sy.com/svn/spring/trunk"
ESVN_PATCHES="spring-svn_coexist_with_release.patch"

LICENSE="GPL-2"
SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE="debug"

RDEPEND="
	!virtual/game_spring
	!games-strategy/taspring-linux-release
	>=dev-lang/python-2.4
	>=dev-libs/boost-1.34
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
	if ! has_version \>=dev-libs/boost-1.34.0 ; then
		my_depend_with_use dev-libs/boost threads
	fi
	${MY_DEPEND_WITH_USE} || die "Some dependencies need different use flags. Package setup failed."
}

src_compile () {
	ewarn "This ebuild installs directly from a development repository."
	ewarn "The code might not even compile some times."
	einfo "If anything is weird, please file a bug report at ${HOMEPAGE}."
	MY_LIBDIR=${GAMES_PREFIX}/$(get_libdir)
	scons configure \
		$(use debug && echo debug=1) \
		prefix="/usr" \
		installprefix="${D}usr/" \
		datadir="${GAMES_DATADIR##/usr/}/${PN}" \
		bindir="${GAMES_BINDIR##/usr/}" \
		libdir="${MY_LIBDIR##/usr/}/${PN}" \
		strip=0 \
		|| die "configuration failed"
	scons || die "build failed"
}

src_install () {
	scons install || die "install failed"
	cd "${D%%/}${GAMES_BINDIR}"
	mv spring ${PN}

	newicon "${FILESDIR}/spring.png" ${PN}.png
	make_desktop_entry ${PN} "Spring RTS - svn" ${PN}.png

	insinto /etc/${PN}
	echo '$HOME'"/.${PN}" > ${WORKDIR}/datadir
	echo "${GAMES_DATADIR}/${PN}" >> ${WORKDIR}/datadir
	doins ${WORKDIR}/datadir

	prepgamesdirs
}

