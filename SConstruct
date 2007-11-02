s_pref =  'src/'
s_source = [s_pref+'Main.cpp',s_pref+'frame.cpp',s_pref+'ConfigHandler.cpp',s_pref+'errorhandler.cpp']
s_source+= [s_pref+'tab_abstract.cpp',s_pref+'tab_audio.cpp',s_pref+'tab_debug.cpp',s_pref+'tab_general.cpp',s_pref+'tab_mouse.cpp',s_pref+'tab_rendering.cpp',s_pref+'tab_video.cpp']
s_win = [s_pref+'RegHandler.cpp']
s_mac = ['']
s_linux = [s_pref+'DotfileHandler.cpp']

s_source += s_linux
s_env=Environment()
s_env.ParseConfig('wx2.8.6-static/wx-config --cxxflags --libs core,base')
#print s_env['LIBPATH']
settings = s_env.Program(target='Settings++',source=s_source,LIBS=s_env['LIBS'],LIBPATH=s_env['LIBPATH'])
#settings = s_env.Program(target='Settings++',source=s_source,LIBS=['wx_gtk2u_core-2.8','wx_baseu-2.8'],LIBPATH=['lib/linux/','wxheaders/'],CPPPATH='wxheaders')
Default(settings)



