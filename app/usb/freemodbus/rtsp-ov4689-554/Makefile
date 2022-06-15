INCLUDE=-I./include/
LIBS=./lib/libmpi.a ./lib/libVoiceEngine.a ./lib/libvqev2.a ./lib/libresampler.a ./lib/libdvqe.a ./lib/libisp.a ./lib/libsns_ov4689.a ./lib/lib_cmoscfg.a ./lib/lib_iniparser.a ./lib/lib_hiae.a ./lib/lib_hiawb.a ./lib/lib_hiaf.a ./lib/lib_hidefog.a -lpthread -lm
all:rtsp
rtsp:
	arm-hisiv300-linux-gcc -o rtsp -Dhi3516a -DHICHIP=0x3516A100 -DSENSOR_TYPE=OMNIVISION_OV4689_MIPI_4M_30FPS -DHI_DEBUG -DHI_XXXX -DISP_V2 -DHI_MIPI_ENABLE -DHI_ACODEC_TYPE_INNER -mcpu=cortex-a7 -mfloat-abi=softfp -mfpu=neon-vfpv4 -ffunction-sections main.c ringfifo.c rtputils.c rtspservice.c rtsputils.c loadbmp.c sample_comm_audio.c sample_comm_isp.c sample_comm_sys.c sample_comm_vda.c sample_comm_venc.c sample_comm_vi.c sample_comm_vo.c sample_comm_vpss.c sample_venc.c $(INCLUDE) $(LIBS)
clean:
	rm -rfv rtsp