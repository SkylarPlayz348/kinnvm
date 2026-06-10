# ScummVM Kindle Port

Original README [here](./README-original.md)

## Requirements:
- Kindle with firmware version 5.16.3+
- At least 2-4 GBs(room for ScummVM + dependencies and a game or two)

## Issues

Report all issues here on github

## Building

Building documentation will be made later but all needed to libraries and headers are in `kindle-libs`.

`FluidSynth` as its optional you will need to build and install on your own. It is confirmed to work and is in any release on from this repo.

`SDL-EP` for this you will need my SDL2 port SDL-EP not much is out there but building and installing to your sysroot is what you need to do

a lot of this is manual but that will change when I redo ScummVMs build system.

**Coming Soon**

## Contributing

All PRs are welcome and wanted.

## Compatibility

See [compatibility.md](./compatibility.md)

## AI Disclosure

I have no experience with Kindle's audio backend so with the help of AI and [kbarni's KinAMP](https://github.com/kbarni/KinAMP) I was able to take advantage of part of how it handles the audio backend. Also with that I was able to integrate FluidSynth into the project.

## Credits

[KinAMP](https://github.com/kbarni/KinAMP) - Used for Audio
