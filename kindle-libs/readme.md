# Kindle Libraries

This is just to have a unified build system.

### Need to Cross-compile (not on Kindle device):

**libmpeg2 0.5.1** — [archive.org mirror](https://web.archive.org/web/20240518094128/https://libmpeg2.sourceforge.io/files/libmpeg2-0.5.1.tar.gz)

— Headers: copy `mpeg2dec/mpeg2.h` from source or host into sysroot `usr/include/mpeg2dec/`

**giflib 5.2.2** — [sourceforge](https://sourceforge.net/projects/giflib/files/giflib-5.x/giflib-5.2.2.tar.gz/download) (plain curl works)
— Headers: `gif_lib.h` copied from source into sysroot `usr/include/`

**musepack r475** — [musepack.net](https://files.musepack.net/source/musepack_src_r475.tar.gz)

Headers: `include/mpc/*.h` from source copied into sysroot `usr/include/mpc/`

### On Kindle but missing headers in sysroot:

**libjpeg-turbo 3.1.0** — [github.com](https://github.com/libjpeg-turbo/libjpeg-turbo/releases/download/3.1.0/libjpeg-turbo-3.1.0.tar.gz)

Headers: `jpeglib.h`, `jconfig.h`, `jmorecfg.h`, `jerror.h` from source copied into sysroot `usr/include/`
