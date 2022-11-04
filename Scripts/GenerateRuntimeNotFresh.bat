@echo off
pushd ..
cmake . -BIntermediate -DENABLE_EDITOR=OFF
popd