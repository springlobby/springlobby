s_pref =  'obj/linux/'
s_source = [s_pref+'Main.cpp',s_pref+'frame.cpp',s_pref+'configHandler.cpp']
s_source+= [s_pref+'tab_abstract.cpp',s_pref+'tab_audio.cpp',s_pref+'tab_debug.cpp',s_pref+'tab_mouse.cpp',s_pref+'tab_render_detail.cpp',s_pref+'tab_quality_video.cpp']



BuildDir('obj/linux/','src/',duplicate=0)
s_env=Environment()
s_env.Append(CCFLAGS=' -g')

s_env.ParseConfig('wx-config --version=2.8 --cxxflags --libs core,base')
#s_env.Prepend(_LIBFLAGS=['-Wl','-Bstatic'])
settings = s_env.Program(target='bin/linux/Settings++.bin',source=s_source)
Default(settings)



