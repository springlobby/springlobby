#!/usr/bin/env bash

set -e

cd /var/lib/buildbot/master/springlobby/springlobby-git-repo/
sudo -u buildbot git-pull
sudo /etc/init.d/buildbot restart
