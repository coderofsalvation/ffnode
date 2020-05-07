#!/bin/bash
export FFMPEG_VERSION=4.2.2
sudo apk add gnutls-dev zlib-dev yasm-dev lame-dev libogg-dev \
    x264-dev libvpx-dev libvorbis-dev x265-dev freetype-dev \
    libass-dev libwebp-dev rtmpdump-dev libtheora-dev opus-dev
#sudo apk add --no-cache --virtual .build-dependencies \
#    build-base coreutils tar bzip2 x264 gnutls nasm && \
    wget -O- http://ffmpeg.org/releases/ffmpeg-${FFMPEG_VERSION}.tar.gz | tar xzC /tmp && \
    cd /tmp/ffmpeg-${FFMPEG_VERSION} && \
    ./configure --bindir="/usr/bin" \
                --enable-version3 \
                --enable-gpl \
                --enable-nonfree \
                --enable-libmp3lame \
                --enable-libx264 \
                --enable-libx265 \
				--enable-ffplay \
				--enable-ladspa \
	     		--enable-openssl  \
               --enable-libvpx \
               --enable-libtheora \
               --enable-libvorbis \
               --enable-libopus \
               --enable-libass \
                --enable-libwebp \
                --enable-librtmp \
                --enable-postproc \
                --enable-avresample \
                --enable-libfreetype \
                --enable-gnutls && \
                #--disable-debug && \
    sudo make && \
    sudo make install && \
    sudo make distclean 
