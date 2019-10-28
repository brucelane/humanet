#!/bin/bash

# Clones dependencies
# Inspired by ObviousJim and ofxTimeline "clone_addons.sh" script

cd ../../../addons/

if [ -z $1 ]; then
    PREFIX="git clone https://github.com/"
else
    PREFIX="git clone git@github.com:"
fi

${PREFIX}julapy/ofxFFT.git

${PREFIX}Kj1/ofxSpout2.git

echo "Done cloning addons! :)"
