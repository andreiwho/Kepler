@echo off
pushd ..
cmake . -BIntermediate -DENABLE_EDITOR=ON
popd
