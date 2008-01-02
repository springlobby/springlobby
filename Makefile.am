## the following clause may NOT be split, b/c of autoreconf 2.61 and older
## the --install requires automake 1.10 which was released in 2006
## the --install can be edited out to use older automake
ACLOCAL_AMFLAGS = -I m4 --install

AM_CPPFLAGS = \
	$(WX_CPPFLAGS)

# strict aliasing check is disabled, because wxWidgets headers have problems
AM_CXXFLAGS = \
	-Wall \
	-Wno-strict-aliasing

## if you don't have the required version of autoconf, comment out the
## following clause, in which case you may NOT distribute the result!
dist_doc_DATA = \
	AUTHORS \
	ChangeLog \
	COPYING \
	INSTALL \
	NEWS \
	README \
	THANKS

bin_PROGRAMS = \
	springlobby \
	springsettings

springlobby_LDADD = \
	$(WX_LIBS)

springsettings_LDADD = \
	$(WX_LIBS)

if USE_WINDRES
RESOURCECOMPILE = $(WINDRES) -I $(srcdir)/src $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
    $(AM_CPPFLAGS) $(CPPFLAGS) # $(AM_CXXFLAGS) $(CXXFLAGS)
.rc.o:
	$(RESOURCECOMPILE) -o $@ -i $<
endif

# filter-out is a GNU make extension, so we get a warning from automake
# and we can not use -Werror in AM_INIT_AUTOMAKE because of this either
# but without this construct we would have a circular dependency and make
# would whine about it on every invocation

revision.cpp: src/revision.cpp.in $(filter-out revision.%, $(springlobby_SOURCES)) $(springlobby_DEPENDENCIES)
	rm -f $@
	sed "s/REV[^\"]*/REV SVN `LC_ALL=C $(SVNVERSION) $(top_srcdir) /trunk || echo svnversion failed or not found`/" < $< > $@

clean-local:
	rm -f revision.cpp

# the following line is searched for in a script to add filenames under it
springlobby_SOURCES = \
	revision.cpp \
	src/springunitsynclib.cpp \
	src/springunitsynclib.h \
	src/revision.cpp.in \
	src/revision.h \
	src/unitsyncthread.cpp \
	src/unitsyncthread.h \
	src/maplistctrl.cpp \
	src/maplistctrl.h \
	src/mapselectdialog.cpp \
	src/mapselectdialog.h \
	src/chatlog.h \
	src/chatlog.cpp \
	src/images/broom.xpm \
	src/images/bot_ingame.xpm \
	src/images/bot_broom.xpm \
	src/images/chanop_ingame.xpm \
	src/images/chanop_away.xpm \
	src/images/chanop_broom.xpm \
	src/images/chanop.xpm \
	src/images/admin_broom.xpm \
	src/images/host_spectator.xpm \
	src/images/host.xpm \
	src/images/not_found_icon.xpm \
	src/images/reload_map.xpm \
	src/images/download_map.xpm \
	src/images/up_downsel.xpm \
	src/images/upsel_down.xpm \
	src/images/up_down.xpm \
	src/images/player.xpm \
	src/ibattle.cpp \
	src/ibattle.h \
	src/nonportable.h \
	src/addbotdialog.cpp \
	src/addbotdialog.h \
	src/agreementdialog.cpp \
	src/agreementdialog.h \
	src/base64.cpp \
	src/base64.h \
	src/battle.cpp \
	src/battle.h \
	src/battlelistctrl.cpp \
	src/battlelistctrl.h \
	src/battlelistfilter.cpp \
	src/battlelistfilter.h \
	src/battlelist.cpp \
	src/battlelist.h \
	src/battlelisttab.cpp \
	src/battlelisttab.h \
	src/battleroomlistctrl.cpp \
	src/battleroomlistctrl.h \
	src/battlemaptab.cpp \
	src/battlemaptab.h \
	src/battleoptionstab.cpp \
	src/battleoptionstab.h \
	src/battleroomtab.cpp \
	src/battleroomtab.h \
	src/boost/md5.cpp \
	src/boost/md5.hpp \
	src/channel.cpp \
	src/channel.h \
	src/channellist.cpp \
	src/channellist.h \
	src/chatoptionstab.cpp \
	src/chatoptionstab.h \
	src/chatpanel.cpp \
	src/chatpanel.h \
	src/connectwindow.cpp \
	src/connectwindow.h \
	src/flagimagedata.h \
	src/flagimages.cpp \
	src/flagimages.h \
	src/hostbattledialog.h \
	src/hostbattledialog.cpp \
	src/iconimagelist.cpp \
	src/iconimagelist.h \
	src/iunitsync.h \
	src/mainchattab.cpp \
	src/mainchattab.h \
	src/mainjoinbattletab.cpp \
	src/mainjoinbattletab.h \
	src/mainoptionstab.cpp \
	src/mainoptionstab.h \
	src/mainsingleplayertab.cpp \
	src/mainsingleplayertab.h \
	src/mainwindow.cpp \
	src/mainwindow.h \
	src/mapctrl.cpp \
	src/mapctrl.h \
	src/nicklistctrl.cpp \
	src/nicklistctrl.h \
	src/server.cpp \
	src/serverevents.cpp \
	src/serverevents.h \
	src/server.h \
	src/settings.cpp \
	src/settings.h \
	src/socket.cpp \
	src/socket.h \
	src/spring.cpp \
	src/spring.h \
	src/singleplayerbattle.cpp \
	src/singleplayerbattle.h \
	src/singleplayertab.cpp \
	src/singleplayertab.h \
	src/springlobbyapp.cpp \
	src/springlobbyapp.h \
	src/springoptionstab.cpp \
	src/springoptionstab.h \
	src/springprocess.cpp \
	src/springprocess.h \
	src/springunitsync.cpp \
	src/springunitsync.h \
	src/stacktrace.cpp \
	src/stacktrace.h \
	src/tasserver.cpp \
	src/tasserver.h \
	src/ui.cpp \
	src/ui.h \
	src/uiutils.cpp \
	src/uiutils.h \
	src/user.cpp \
	src/user.h \
	src/userlist.cpp \
	src/userlist.h \
	src/utils.cpp \
	src/utils.h \
	src/images/start_ally.xpm \
	src/images/start_enemy.xpm \
	src/images/start_unused.xpm \
	src/images/admin_away.xpm \
	src/images/admin_ingame.xpm \
	src/images/admin.xpm \
	src/images/arm.xpm \
	src/images/away.xpm \
	src/images/battle_list.xpm \
	src/images/battle_map.xpm \
	src/images/battle_settings.xpm \
	src/images/battle.xpm \
	src/images/bot.xpm \
	src/images/channel.xpm \
	src/images/chat_icon.xpm \
	src/images/closed_game.xpm \
	src/images/closed_pw_game.xpm \
	src/images/close_hi.xpm \
	src/images/close.xpm \
	src/images/colourbox.xpm \
	src/images/connect.xpm \
	src/images/core.xpm \
	src/images/down.xpm \
	src/images/exists.xpm \
	src/images/ingame.xpm \
	src/images/join_icon.xpm \
	src/images/nexists.xpm \
	src/images/nready_q.xpm \
	src/images/nready_unsync.xpm \
	src/images/open_game.xpm \
	src/images/open_pw_game.xpm \
	src/images/options_icon.xpm \
	src/images/rank0.xpm \
	src/images/rank1.xpm \
	src/images/rank2.xpm \
	src/images/rank3.xpm \
	src/images/rank4.xpm \
        src/images/rank5.xpm \
        src/images/rank6.xpm \
	src/images/ready_q.xpm \
	src/images/ready_unsync.xpm \
	src/images/singleplayer_icon.xpm \
	src/images/select_icon.xpm \
	src/images/server.xpm \
	src/images/spectator.xpm \
	src/images/spring.xpm \
	src/images/springlobby.xpm \
	src/images/started_game.xpm \
	src/images/unknown_flag.xpm \
	src/images/up.xpm \
	src/images/userchat.xpm \
	src/images/flags/AD.xpm \
	src/images/flags/AE.xpm \
	src/images/flags/AF.xpm \
	src/images/flags/AG.xpm \
	src/images/flags/AI.xpm \
	src/images/flags/AL.xpm \
	src/images/flags/AM.xpm \
	src/images/flags/AN.xpm \
	src/images/flags/AO.xpm \
	src/images/flags/AR.xpm \
	src/images/flags/AS.xpm \
	src/images/flags/AT.xpm \
	src/images/flags/AU.xpm \
	src/images/flags/AW.xpm \
	src/images/flags/AX.xpm \
	src/images/flags/AZ.xpm \
	src/images/flags/BA.xpm \
	src/images/flags/BB.xpm \
	src/images/flags/BD.xpm \
	src/images/flags/BE.xpm \
	src/images/flags/BF.xpm \
	src/images/flags/BG.xpm \
	src/images/flags/BH.xpm \
	src/images/flags/BI.xpm \
	src/images/flags/BJ.xpm \
	src/images/flags/BM.xpm \
	src/images/flags/BN.xpm \
	src/images/flags/BO.xpm \
	src/images/flags/BR.xpm \
	src/images/flags/BS.xpm \
	src/images/flags/BT.xpm \
	src/images/flags/BV.xpm \
	src/images/flags/BW.xpm \
	src/images/flags/BY.xpm \
	src/images/flags/BZ.xpm \
	src/images/flags/CA.xpm \
	src/images/flags/CC.xpm \
	src/images/flags/CD.xpm \
	src/images/flags/CF.xpm \
	src/images/flags/CG.xpm \
	src/images/flags/CH.xpm \
	src/images/flags/CI.xpm \
	src/images/flags/CK.xpm \
	src/images/flags/CL.xpm \
	src/images/flags/CM.xpm \
	src/images/flags/CN.xpm \
	src/images/flags/CO.xpm \
	src/images/flags/CR.xpm \
	src/images/flags/CS.xpm \
	src/images/flags/CU.xpm \
	src/images/flags/CV.xpm \
	src/images/flags/CX.xpm \
	src/images/flags/CY.xpm \
	src/images/flags/CZ.xpm \
	src/images/flags/DE.xpm \
	src/images/flags/DJ.xpm \
	src/images/flags/DK.xpm \
	src/images/flags/DM.xpm \
	src/images/flags/DO.xpm \
	src/images/flags/DZ.xpm \
	src/images/flags/EC.xpm \
	src/images/flags/EE.xpm \
	src/images/flags/EG.xpm \
	src/images/flags/EH.xpm \
	src/images/flags/ER.xpm \
	src/images/flags/ES.xpm \
	src/images/flags/ET.xpm \
	src/images/flags/FAM.xpm \
	src/images/flags/FI.xpm \
	src/images/flags/FJ.xpm \
	src/images/flags/FK.xpm \
	src/images/flags/FM.xpm \
	src/images/flags/FO.xpm \
	src/images/flags/FR.xpm \
	src/images/flags/GA.xpm \
	src/images/flags/GB.xpm \
	src/images/flags/GD.xpm \
	src/images/flags/GE.xpm \
	src/images/flags/GF.xpm \
	src/images/flags/GH.xpm \
	src/images/flags/GI.xpm \
	src/images/flags/GL.xpm \
	src/images/flags/GM.xpm \
	src/images/flags/GN.xpm \
	src/images/flags/GP.xpm \
	src/images/flags/GQ.xpm \
	src/images/flags/GR.xpm \
	src/images/flags/GS.xpm \
	src/images/flags/GT.xpm \
	src/images/flags/GU.xpm \
	src/images/flags/GW.xpm \
	src/images/flags/GY.xpm \
	src/images/flags/HK.xpm \
	src/images/flags/HM.xpm \
	src/images/flags/HN.xpm \
	src/images/flags/HR.xpm \
	src/images/flags/HT.xpm \
	src/images/flags/HU.xpm \
	src/images/flags/ID.xpm \
	src/images/flags/IE.xpm \
	src/images/flags/IL.xpm \
	src/images/flags/IN.xpm \
	src/images/flags/IO.xpm \
	src/images/flags/IQ.xpm \
	src/images/flags/IR.xpm \
	src/images/flags/IS.xpm \
	src/images/flags/IT.xpm \
	src/images/flags/JM.xpm \
	src/images/flags/JO.xpm \
	src/images/flags/JP.xpm \
	src/images/flags/KE.xpm \
	src/images/flags/KG.xpm \
	src/images/flags/KH.xpm \
	src/images/flags/KI.xpm \
	src/images/flags/KM.xpm \
	src/images/flags/KN.xpm \
	src/images/flags/KP.xpm \
	src/images/flags/KR.xpm \
	src/images/flags/KW.xpm \
	src/images/flags/KY.xpm \
	src/images/flags/KZ.xpm \
	src/images/flags/LA.xpm \
	src/images/flags/LB.xpm \
	src/images/flags/LC.xpm \
	src/images/flags/LI.xpm \
	src/images/flags/LK.xpm \
	src/images/flags/LR.xpm \
	src/images/flags/LS.xpm \
	src/images/flags/LT.xpm \
	src/images/flags/LU.xpm \
	src/images/flags/LV.xpm \
	src/images/flags/LY.xpm \
	src/images/flags/MA.xpm \
	src/images/flags/MC.xpm \
	src/images/flags/MD.xpm \
	src/images/flags/ME.xpm \
	src/images/flags/MG.xpm \
	src/images/flags/MH.xpm \
	src/images/flags/MK.xpm \
	src/images/flags/ML.xpm \
	src/images/flags/MM.xpm \
	src/images/flags/MN.xpm \
	src/images/flags/MO.xpm \
	src/images/flags/MP.xpm \
	src/images/flags/MQ.xpm \
	src/images/flags/MR.xpm \
	src/images/flags/MS.xpm \
	src/images/flags/MT.xpm \
	src/images/flags/MU.xpm \
	src/images/flags/MV.xpm \
	src/images/flags/MW.xpm \
	src/images/flags/MX.xpm \
	src/images/flags/MY.xpm \
	src/images/flags/MZ.xpm \
	src/images/flags/NA.xpm \
	src/images/flags/NC.xpm \
	src/images/flags/NE.xpm \
	src/images/flags/NF.xpm \
	src/images/flags/NG.xpm \
	src/images/flags/NI.xpm \
	src/images/flags/NL.xpm \
	src/images/flags/NO.xpm \
	src/images/flags/NP.xpm \
	src/images/flags/NR.xpm \
	src/images/flags/NU.xpm \
	src/images/flags/NZ.xpm \
	src/images/flags/OM.xpm \
	src/images/flags/PA.xpm \
	src/images/flags/PE.xpm \
	src/images/flags/PF.xpm \
	src/images/flags/PG.xpm \
	src/images/flags/PH.xpm \
	src/images/flags/PK.xpm \
	src/images/flags/PL.xpm \
	src/images/flags/PM.xpm \
	src/images/flags/PN.xpm \
	src/images/flags/PR.xpm \
	src/images/flags/PS.xpm \
	src/images/flags/PT.xpm \
	src/images/flags/PW.xpm \
	src/images/flags/PY.xpm \
	src/images/flags/QA.xpm \
	src/images/flags/RE.xpm \
	src/images/flags/RO.xpm \
	src/images/flags/RS.xpm \
	src/images/flags/RU.xpm \
	src/images/flags/RW.xpm \
	src/images/flags/SA.xpm \
	src/images/flags/SB.xpm \
	src/images/flags/SC.xpm \
	src/images/flags/SD.xpm \
	src/images/flags/SE.xpm \
	src/images/flags/SG.xpm \
	src/images/flags/SH.xpm \
	src/images/flags/SI.xpm \
	src/images/flags/SJ.xpm \
	src/images/flags/SK.xpm \
	src/images/flags/SL.xpm \
	src/images/flags/SM.xpm \
	src/images/flags/SN.xpm \
	src/images/flags/SO.xpm \
	src/images/flags/SR.xpm \
	src/images/flags/ST.xpm \
	src/images/flags/SV.xpm \
	src/images/flags/SY.xpm \
	src/images/flags/SZ.xpm \
	src/images/flags/TC.xpm \
	src/images/flags/TD.xpm \
	src/images/flags/TF.xpm \
	src/images/flags/TG.xpm \
	src/images/flags/TH.xpm \
	src/images/flags/TJ.xpm \
	src/images/flags/TK.xpm \
	src/images/flags/TL.xpm \
	src/images/flags/TM.xpm \
	src/images/flags/TN.xpm \
	src/images/flags/TO.xpm \
	src/images/flags/TR.xpm \
	src/images/flags/TT.xpm \
	src/images/flags/TV.xpm \
	src/images/flags/TW.xpm \
	src/images/flags/TZ.xpm \
	src/images/flags/UA.xpm \
	src/images/flags/UG.xpm \
	src/images/flags/UM.xpm \
	src/images/flags/US.xpm \
	src/images/flags/UY.xpm \
	src/images/flags/UZ.xpm \
	src/images/flags/VA.xpm \
	src/images/flags/VC.xpm \
	src/images/flags/VE.xpm \
	src/images/flags/VG.xpm \
	src/images/flags/VI.xpm \
	src/images/flags/VN.xpm \
	src/images/flags/VU.xpm \
	src/images/flags/WF.xpm \
	src/images/flags/WS.xpm \
	src/images/flags/YE.xpm \
	src/images/flags/YT.xpm \
	src/images/flags/ZA.xpm \
	src/images/flags/ZM.xpm \
	src/images/flags/ZW.xpm \
	src/customlistctrl.h \
	src/customlistctrl.cpp 


# TODO keep only a single list instead of duplicating the lists below
# note, integrated has everything but main.cpp
springlobby_SOURCES += \
	src/settings++/se_utils.h \
	src/settings++/se_utils.cpp \
	src/settings++/frame.cpp \
	src/settings++/main.h \
	src/settings++/tab_ui.cpp \
	src/settings++/tab_render_detail.cpp \
	src/settings++/tab_quality_video.h \
	src/settings++/frame.h \
	src/settings++/tab_audio.h \
	src/settings++/se_settings.h \
	src/settings++/tab_debug.cpp \
	src/settings++/tab_debug.h \
	src/settings++/se_settings.cpp \
	src/settings++/tab_audio.cpp \
	src/settings++/presets.h \
	src/settings++/presets.cpp \
	src/settings++/tab_abstract.cpp \
	src/settings++/Defs.hpp \
	src/settings++/tab_simple.cpp \
	src/settings++/tab_quality_video.cpp \
	src/settings++/tab_ui.h \
	src/settings++/tab_simple.h \
	src/images/springsettings.xpm \
	src/settings++/tab_render_detail.h \
	src/settings++/tab_abstract.h \
	src/settings++/panel_pathoption.h \
	src/settings++/panel_pathoption.cpp \
	src/settings++/custom_dialogs.h \
	src/settings++/custom_dialogs.cpp \
	src/settings++/helpmenufunctions.h \
	src/settings++/helpmenufunctions.cpp 

if USE_WINDRES
	springlobby_SOURCES += \
		src/springlobby.rc
endif


springsettings_SOURCES = \
	revision.cpp \
	src/revision.cpp.in \
	src/revision.h \
	src/base64.cpp \
	src/base64.h \
	src/utils.cpp \
	src/utils.h \
	src/stacktrace.cpp \
	src/stacktrace.h \
	src/boost/md5.cpp \
	src/boost/md5.hpp \
	src/springunitsynclib.cpp \
	src/springunitsynclib.h \
	src/settings++/se_utils.cpp \
	src/settings++/se_utils.h \
	src/settings++/frame.cpp \
	src/settings++/main.h \
	src/settings++/tab_ui.cpp \
	src/settings++/tab_render_detail.cpp \
	src/settings++/tab_quality_video.h \
	src/settings++/frame.h \
	src/settings++/tab_audio.h \
	src/settings++/se_settings.h \
	src/settings++/tab_debug.cpp \
	src/settings++/tab_debug.h \
	src/settings++/se_settings.cpp \
	src/settings++/tab_audio.cpp \
	src/settings++/presets.h \
	src/settings++/presets.cpp \
	src/settings++/tab_abstract.cpp \
	src/settings++/Main.cpp \
	src/settings++/Defs.hpp \
	src/settings++/tab_simple.cpp \
	src/settings++/tab_quality_video.cpp \
	src/settings++/tab_ui.h \
	src/settings++/tab_simple.h \
	src/images/springsettings.xpm \
	src/settings++/tab_render_detail.h \
	src/settings++/tab_abstract.h \
	src/settings++/panel_pathoption.h \
	src/settings++/panel_pathoption.cpp	\
	src/settings++/custom_dialogs.h \
	src/settings++/custom_dialogs.cpp \
	src/settings++/helpmenufunctions.h \
	src/settings++/helpmenufunctions.cpp 


if USE_WINDRES
	springsettings_SOURCES += \
		src/springlobby.rc
endif

