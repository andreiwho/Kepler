@echo off
pushd ..
cmake . -BIntermediate --fresh -DENABLE_EDITOR=OFF
popd