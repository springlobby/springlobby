s_pref =  'obj/linux/'
s_source = [s_pref+'Main.cpp',s_pref+'frame.cpp',s_pref+'spring/ConfigHandler.cpp',s_pref+'spring/errorhandler.cpp']
s_source+= [s_pref+'tab_abstract.cpp',s_pref+'tab_audio.cpp',s_pref+'tab_debug.cpp',s_pref+'tab_general.cpp',s_pref+'tab_mouse.cpp',s_pref+'tab_render_detail.cpp',s_pref+'tab_video.cpp']
s_win = [s_pref+'spring/RegHandler.cpp']
s_mac = ['']
s_linux = [s_pref+'spring/DotfileHandler.cpp']

s_source += s_linux
BuildDir('obj/linux/','src/',duplicate=0)
s_env=Environment()
s_env.ParseConfig('wx2.8.6/bin/wx-config --prefix=wx2.8.6 --exec-prefix=wx2.8.6 --cxxflags --libs core,base')

#s_env.Prepend(_LIBFLAGS=['-Wl','-Bstatic'])
settings = s_env.Program(target='bin/linux/Settings++.bin',source=s_source)
Default(settings)



