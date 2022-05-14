static Bool FFD_CanHandleURL(GF_InputService *plug, const char *url)
{
	Bool has_audio, has_video;
	s32 i;
 	AVFormatContext *ctx;
 	AVOutputFormat *fmt_out;
 	Bool ret = GF_FALSE;
	char *ext, szName[1024], szExt[20];
 	const char *szExtList;
 	FFDemux *ffd;
 	if (!plug || !url)
		return GF_FALSE;
	/*disable RTP/RTSP from ffmpeg*/
	if (!strnicmp(url, "rtsp://", 7)) return GF_FALSE;
	if (!strnicmp(url, "rtspu://", 8)) return GF_FALSE;
	if (!strnicmp(url, "rtp://", 6)) return GF_FALSE;
	if (!strnicmp(url, "plato://", 8)) return GF_FALSE;
	if (!strnicmp(url, "udp://", 6)) return GF_FALSE;
	if (!strnicmp(url, "tcp://", 6)) return GF_FALSE;
	if (!strnicmp(url, "data:", 5)) return GF_FALSE;
 
 	ffd = (FFDemux*)plug->priv;
 
	if (strlen(url) >= sizeof(szName))
		return GF_FALSE;

 	strcpy(szName, url);
 	ext = strrchr(szName, '#');
 	if (ext) ext[0] = 0;
	ext = strrchr(szName, '?');
	if (ext) ext[0] = 0;

 	ext = strrchr(szName, '.');
 	if (ext && strlen(ext) > 19) ext = NULL;
 
	if (ext && strlen(ext) > 1 && strlen(ext) <= sizeof(szExt)) {
 		strcpy(szExt, &ext[1]);
 		strlwr(szExt);
 #ifndef FFMPEG_DEMUX_ENABLE_MPEG2TS
		if (strstr("ts m2t mts dmb trp", szExt) ) return GF_FALSE;
#endif

		/*note we forbid ffmpeg to handle files we support*/
		if (!strcmp(szExt, "mp4") || !strcmp(szExt, "mpg4") || !strcmp(szExt, "m4a") || !strcmp(szExt, "m21")
		        || !strcmp(szExt, "m4v") || !strcmp(szExt, "m4a")
		        || !strcmp(szExt, "m4s") || !strcmp(szExt, "3gs")
		        || !strcmp(szExt, "3gp") || !strcmp(szExt, "3gpp") || !strcmp(szExt, "3gp2") || !strcmp(szExt, "3g2")
		        || !strcmp(szExt, "mp3")
		        || !strcmp(szExt, "ac3")
		        || !strcmp(szExt, "amr")
		        || !strcmp(szExt, "bt") || !strcmp(szExt, "wrl") || !strcmp(szExt, "x3dv")
		        || !strcmp(szExt, "xmt") || !strcmp(szExt, "xmta") || !strcmp(szExt, "x3d")

		        || !strcmp(szExt, "jpg") || !strcmp(szExt, "jpeg") || !strcmp(szExt, "png")
		   ) return GF_FALSE;

		/*check any default stuff that should work with ffmpeg*/
		{
			u32 i;
			for (i = 0 ; FFD_MIME_TYPES[i]; i+=3) {
				if (gf_service_check_mime_register(plug, FFD_MIME_TYPES[i], FFD_MIME_TYPES[i+1], FFD_MIME_TYPES[i+2], ext))
					return GF_TRUE;
			}
		}
	}

	ffd_parse_options(ffd, url);

	ctx = NULL;
	if (open_file(&ctx, szName, NULL, ffd->options ? &ffd->options : NULL)<0) {
		AVInputFormat *av_in = NULL;
		/*some extensions not supported by ffmpeg*/
		if (ext && !strcmp(szExt, "cmp")) av_in = av_find_input_format("m4v");

		if (open_file(&ctx, szName, av_in, ffd->options ? &ffd->options : NULL)<0) {
			return GF_FALSE;
		}
	}

	if (!ctx) goto exit;
	if (av_find_stream_info(ctx) <0) goto exit;

	/*figure out if we can use codecs or not*/
	has_video = has_audio = GF_FALSE;
	for(i = 0; i < (s32)ctx->nb_streams; i++) {
		AVCodecContext *enc = ctx->streams[i]->codec;
		switch(enc->codec_type) {
		case AVMEDIA_TYPE_AUDIO:
			if (!has_audio) has_audio = GF_TRUE;
			break;
		case AVMEDIA_TYPE_VIDEO:
			if (!has_video) has_video= GF_TRUE;
			break;
		default:
			break;
		}
	}
	if (!has_audio && !has_video) goto exit;
	ret = GF_TRUE;
#if ((LIBAVFORMAT_VERSION_MAJOR == 52) && (LIBAVFORMAT_VERSION_MINOR <= 47)) || (LIBAVFORMAT_VERSION_MAJOR < 52)
	fmt_out = guess_stream_format(NULL, url, NULL);
#else
	fmt_out = av_guess_format(NULL, url, NULL);
#endif
	if (fmt_out) gf_service_register_mime(plug, fmt_out->mime_type, fmt_out->extensions, fmt_out->name);
	else {
		ext = strrchr(szName, '.');
		if (ext) {
			strcpy(szExt, &ext[1]);
			strlwr(szExt);

			szExtList = gf_modules_get_option((GF_BaseInterface *)plug, "MimeTypes", "application/x-ffmpeg");
			if (!szExtList) {
				gf_service_register_mime(plug, "application/x-ffmpeg", szExt, "Other Movies (FFMPEG)");
			} else if (!strstr(szExtList, szExt)) {
				u32 len;
				char *buf;
				len = (u32) (strlen(szExtList) + strlen(szExt) + 10);
				buf = (char*)gf_malloc(sizeof(char)*len);
				sprintf(buf, "\"%s ", szExt);
				strcat(buf, &szExtList[1]);
				gf_modules_set_option((GF_BaseInterface *)plug, "MimeTypes", "application/x-ffmpeg", buf);
				gf_free(buf);
			}
		}
	}

exit:
#if FF_API_CLOSE_INPUT_FILE
	if (ctx) av_close_input_file(ctx);
#else
	if (ctx) avformat_close_input(&ctx);
#endif
	return ret;
}
