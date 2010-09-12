
from twisted.application import service
from buildbot.master import BuildMaster

basedir = r'/var/lib/buildbot/master/springlobby'
configfile = r'master.cfg'

application = service.Application('buildmaster')
BuildMaster(basedir, configfile).setServiceParent(application)

