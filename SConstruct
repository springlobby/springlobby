s_pref =  'src/'
s_source = [s_pref+'Main.cpp',s_pref+'Window.cpp',s_pref+'Dialog.cpp',s_pref+'ConfigHandler.cpp',s_pref+'errorhandler.cpp']
s_win = [s_pref+'RegHandler.cpp']
s_mac = ['']
s_linux = [s_pref+'DotfileHandler.cpp']

s_source += s_linux
s_env=Environment()
s_env.ParseConfig('wx-config --version=2.8 --cxxflags --libs')

settings = s_env.Program(target='Settings++',source=s_source,LIBS=s_env['LIBS'],LIBPATH=s_env['LIBPATH'])
Default(settings)
