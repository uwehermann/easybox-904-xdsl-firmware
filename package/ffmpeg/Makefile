
include $(TOPDIR)/rules.mk

PKG_NAME:=ffmpeg
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/ffmpeg
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=FFmpeg vedio and audio decoder library
  DEPENDS:=
endef

define Package/ffmpeg/description
 This package contains a library for vedio and audio decoding
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
	mkdir -p $(STAGING_DIR)/usr/include/ffmpeg
	mkdir -p $(STAGING_DIR)/usr/include/ffmpeg/libavformat
	mkdir -p $(STAGING_DIR)/usr/include/ffmpeg/libavcodec
	mkdir -p $(STAGING_DIR)/usr/include/ffmpeg/libavutil
	$(CP) $(PKG_BUILD_DIR)/libavformat/*.h  $(STAGING_DIR)/usr/include/ffmpeg/libavformat
	$(CP) $(PKG_BUILD_DIR)/libavcodec/*.h   $(STAGING_DIR)/usr/include/ffmpeg/libavcodec
	$(CP) $(PKG_BUILD_DIR)/libavutil/*.h    $(STAGING_DIR)/usr/include/ffmpeg/libavutil
	ln -sf $(STAGING_DIR)/usr/include/ffmpeg/libavformat/avformat.h  $(STAGING_DIR)/usr/include/ffmpeg/avformat.h
	ln -sf $(STAGING_DIR)/usr/include/ffmpeg/libavformat/avio.h      $(STAGING_DIR)/usr/include/ffmpeg/avio.h
	ln -sf $(STAGING_DIR)/usr/include/ffmpeg/libavcodec/avcodec.h    $(STAGING_DIR)/usr/include/ffmpeg/avcodec.h
endef

define Build/Configure
	( cd $(PKG_BUILD_DIR); \
                ./configure \
                        --cross-prefix=mips-linux- \
                        --arch=mips \
                        --enable-shared \
                        --enable-cross-compile \
                        --disable-static \
                        --target-os=linux \
                        --disable-logging \
                        --disable-mmx \
                        --disable-mmx2 \
                        --disable-ssse3 \
                        --disable-armv5te \
                        --disable-armv6 \
                        --disable-iwmmxt \
                        --disable-altivec \
                        --disable-network \
                        --disable-ipv6 \
                        --disable-vhook \
                        --disable-debug \
                        --disable-mpegaudio-hp \
                        --disable-ffserver \
                        --disable-ffplay \
                        --enable-small \
                        --disable-devices \
                        --disable-parsers \
                        --disable-bsfs \
                        --disable-filters \
                        --enable-parser=mpegaudio \
						--disable-encoder=h263p \
						--disable-encoder=ljpeg \
						--disable-encoder=pam \
						--disable-encoder=pbm \
						--disable-encoder=pgm \
						--disable-encoder=pgmyuv \
						--disable-encoder=ppm \
						--disable-encoder=sonic_ls \
						--disable-encoder=libfaac \
						--disable-encoder=libmp3lame \
						--disable-encoder=libtheora \
						--disable-encoder=libvorbis \
						--disable-encoder=libx264 \
						--disable-encoder=libxvid \
						--disable-decoder=aasc \
						--disable-decoder=amv \
						--disable-decoder=avs \
						--disable-decoder=bethsoftvid \
						--disable-decoder=bfi \
						--disable-decoder=c93 \
						--disable-decoder=cavs \
						--disable-decoder=cinepak \
						--disable-decoder=cljr \
						--disable-decoder=cscd \
						--disable-decoder=cyuv \
						--disable-decoder=dsicinvideo \
						--disable-decoder=dxa \
						--disable-decoder=eightbps \
						--disable-decoder=eightsvx_exp \
						--disable-decoder=eightsvx_fib \
						--disable-decoder=escape124 \
						--disable-decoder=flic \
						--disable-decoder=fourxm \
						--disable-decoder=fraps \
						--disable-decoder=h263i \
						--disable-decoder=h264 \
						--disable-decoder=idcin \
						--disable-decoder=indeo2 \
						--disable-decoder=indeo3 \
						--disable-decoder=interplay_video \
						--disable-decoder=kmvc \
						--disable-decoder=loco \
						--disable-decoder=mdec \
						--disable-decoder=mimic \
						--disable-decoder=mjpegb \
						--disable-decoder=mmvideo \
						--disable-decoder=mpeg_xvmc \
						--disable-decoder=mpegvideo \
						--disable-decoder=msrle \
						--disable-decoder=msvideo1 \
						--disable-decoder=mszh \
						--disable-decoder=nuv \
						--disable-decoder=pcx \
						--disable-decoder=ptx \
						--disable-decoder=qdraw \
						--disable-decoder=qpeg \
						--disable-decoder=rl2 \
						--disable-decoder=rpza \
						--disable-decoder=smacker \
						--disable-decoder=smc \
						--disable-decoder=sp5x \
						--disable-decoder=sunrast \
						--disable-decoder=svq3 \
						--disable-decoder=theora \
						--disable-decoder=thp \
						--disable-decoder=tiertexseqvideo \
						--disable-decoder=truemotion1 \
						--disable-decoder=truemotion2 \
						--disable-decoder=tscc \
						--disable-decoder=txd \
						--disable-decoder=ulti \
						--disable-decoder=vb \
						--disable-decoder=vc1 \
						--disable-decoder=vcr1 \
						--disable-decoder=vmdvideo \
						--disable-decoder=vmnc \
						--disable-decoder=vp3 \
						--disable-decoder=vp5 \
						--disable-decoder=vp6 \
						--disable-decoder=vp6a \
						--disable-decoder=vp6f \
						--disable-decoder=vqa \
						--disable-decoder=wmv3 \
						--disable-decoder=wnv1 \
						--disable-decoder=xan_wc3 \
						--disable-decoder=xl \
						--disable-decoder=xsub \
						--disable-decoder=alac \
						--disable-decoder=ape \
						--disable-decoder=atrac3 \
						--disable-decoder=cook \
						--disable-decoder=dca \
						--disable-decoder=dsicinaudio \
						--disable-decoder=imc \
						--disable-decoder=mace3 \
						--disable-decoder=mace6 \
						--disable-decoder=mp3 \
						--disable-decoder=mp3adu \
						--disable-decoder=mp3on4 \
						--disable-decoder=mpc7 \
						--disable-decoder=mpc8 \
						--disable-decoder=mpeg4aac \
						--disable-decoder=nellymoser \
						--disable-decoder=qdm2 \
						--disable-decoder=ra_144 \
						--disable-decoder=ra_288 \
						--disable-decoder=shorten \
						--disable-decoder=smackaud \
						--disable-decoder=truespeech \
						--disable-decoder=tta \
						--disable-decoder=vmdaudio \
						--disable-decoder=wavpack \
						--disable-decoder=ws_snd1 \
						--disable-decoder=liba52 \
						--disable-decoder=libfaad \
						--disable-encoder=asv1 \
						--disable-decoder=asv1 \
						--disable-encoder=asv2 \
						--disable-decoder=asv2 \
						--disable-encoder=bmp \
						--disable-decoder=bmp \
						--disable-encoder=dnxhd \
						--disable-decoder=dnxhd \
						--disable-encoder=dvvideo \
						--disable-decoder=dvvideo \
						--disable-encoder=ffv1 \
						--disable-decoder=ffv1 \
						--disable-encoder=ffvhuff \
						--disable-decoder=ffvhuff \
						--disable-encoder=flashsv \
						--disable-decoder=flashsv \
						--disable-encoder=flv \
						--disable-decoder=flv \
						--disable-encoder=gif \
						--disable-decoder=gif \
						--disable-encoder=h261 \
						--disable-decoder=h261 \
						--disable-encoder=h263 \
						--disable-decoder=h263 \
						--disable-encoder=huffyuv \
						--disable-decoder=huffyuv \
						--disable-encoder=jpegls \
						--disable-decoder=jpegls \
						--disable-encoder=mjpeg \
						--disable-decoder=mjpeg \
						--disable-encoder=mpeg1video \
						--disable-decoder=mpeg1video \
						--disable-encoder=mpeg2video \
						--disable-decoder=mpeg2video \
						--disable-encoder=mpeg4 \
						--disable-decoder=mpeg4 \
						--disable-encoder=msmpeg4v1 \
						--disable-decoder=msmpeg4v1 \
						--disable-encoder=msmpeg4v2 \
						--disable-decoder=msmpeg4v2 \
						--disable-encoder=msmpeg4v3 \
						--disable-decoder=msmpeg4v3 \
						--disable-encoder=png \
						--disable-decoder=png \
						--disable-encoder=qtrle \
						--disable-decoder=qtrle \
						--disable-encoder=rawvideo \
						--disable-decoder=rawvideo \
						--disable-encoder=roq \
						--disable-decoder=roq \
						--disable-encoder=rv10 \
						--disable-decoder=rv10 \
						--disable-encoder=rv20 \
						--disable-decoder=rv20 \
						--disable-encoder=sgi \
						--disable-decoder=sgi \
						--disable-encoder=snow \
						--disable-decoder=snow \
						--disable-encoder=svq1 \
						--disable-decoder=svq1 \
						--disable-encoder=targa \
						--disable-decoder=targa \
						--disable-encoder=tiff \
						--disable-decoder=tiff \
						--disable-encoder=wmv1 \
						--disable-decoder=wmv1 \
						--disable-encoder=wmv2 \
						--disable-decoder=wmv2 \
						--disable-encoder=zlib \
						--disable-decoder=zlib \
						--disable-encoder=zmbv \
						--disable-decoder=zmbv \
						--disable-encoder=ac3 \
						--disable-decoder=ac3 \
						--disable-encoder=flac \
						--disable-decoder=flac \
						--disable-encoder=mp2 \
						--disable-decoder=mp2 \
						--disable-encoder=sonic \
						--disable-decoder=sonic \
						--disable-encoder=vorbis \
						--disable-decoder=vorbis \
						--disable-encoder=wmav1 \
						--disable-decoder=wmav1 \
						--disable-encoder=wmav2 \
						--disable-decoder=wmav2 \
						--disable-encoder=dvbsub \
						--disable-decoder=dvbsub \
						--disable-encoder=dvdsub \
						--disable-decoder=dvdsub \
						--disable-encoder=libamr_nb \
						--disable-decoder=libamr_nb \
						--disable-encoder=libamr_wb \
						--disable-decoder=libamr_wb \
						--disable-encoder=libgsm \
						--disable-decoder=libgsm \
						--disable-encoder=libgsm_ms \
						--disable-decoder=libgsm_ms \
	)
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) BLD_DIR=$(PKG_BUILD_DIR)
endef

define Package/ffmpeg/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ffmpeg                      $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/libavformat/libavformat.so* $(1)/usr/lib/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/libavcodec/libavcodec.so*   $(1)/usr/lib/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/libavutil/libavutil.so*     $(1)/usr/lib/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/libavdevice/libavdevice.so* $(1)/usr/lib/
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ffmpeg             $(1)/usr/bin/
	$(CP) $(PKG_BUILD_DIR)/libavformat/libavformat.so* $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/libavcodec/libavcodec.so*   $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/libavutil/libavutil.so*     $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/libavdevice/libavdevice.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,ffmpeg))
