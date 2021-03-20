#!/bin/bash
# download current agps file
wget http://xtrapath1.izatcloud.net/xtra3grc.bin -O data/xtra3grc.bin
# create spiffs with that file
pio run --target uploadfs