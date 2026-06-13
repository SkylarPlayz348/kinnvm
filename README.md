# KinnVM a ScummVM Kindle Port

Original README [here](./README-original.md)

## Requirements:
- Kindle with firmware version 5.16.3+
- At least 2-4 GBs(room for ScummVM + dependencies and a game or two)

## Issues

Report all issues here on github

## Building

Building ScummVM requires a few steps.

### Building SDL2:

KinnVM runs on the SDL backend and as such runs on my SDL2 kindle port. In my SDL2 port there are two `.cmake` files and each serves a specific purpose. `*-software.cmake` which is the config this

### Build FluidSynth:
FluidSynth Support for KinnVM is easy

### Configuring KinnVM:
below is how to use all

```
configure \
  --host=arm-kindlehf-linux-gnueabihf \
  --prefix=/mnt/us/scummvm \
  --with-sdl-prefix=/home/$USER/x-tools/arm-kindlehf-linux-gnueabihf/arm-kindlehf-linux-gnueabihf/sysroot/usr/local/bin/ \
  --with-fluidsynth-prefix=/home/$USER/x-tools/arm-kindlehf-linux-gnueabihf/arm-kindlehf-linux-gnueabihf/sysroot/usr/TODO
  --disable-debug \
  --disable-alsa
```


## Contributing

All PRs are welcome and wanted.

## Compatibility

See [compatibility.md](./compatibility.md)

## AI Disclosure

I have no experience with Kindle's audio backend so with the help of AI and [kbarni's KinAMP](https://github.com/kbarni/KinAMP) I was able to take advantage of part of how it handles the audio backend. Also with that I was able to integrate FluidSynth into the project.

## Credits

[KinAMP](https://github.com/kbarni/KinAMP) - Used for Audio
