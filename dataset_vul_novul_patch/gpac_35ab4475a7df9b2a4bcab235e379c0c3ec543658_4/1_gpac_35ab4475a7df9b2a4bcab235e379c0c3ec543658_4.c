GF_Err gf_sm_load_init(GF_SceneLoader *load)
{
	GF_Err e = GF_NOT_SUPPORTED;
	char *ext, szExt[50];
	/*we need at least a scene graph*/
	if (!load || (!load->ctx && !load->scene_graph)
#ifndef GPAC_DISABLE_ISOM
	        || (!load->fileName && !load->isom && !(load->flags & GF_SM_LOAD_FOR_PLAYBACK) )
#endif
	   ) return GF_BAD_PARAM;

	if (!load->type) {
#ifndef GPAC_DISABLE_ISOM
		if (load->isom) {
			load->type = GF_SM_LOAD_MP4;
		} else
#endif
		{
			ext = (char *)strrchr(load->fileName, '.');
			if (!ext) return GF_NOT_SUPPORTED;
			if (!stricmp(ext, ".gz")) {
				char *anext;
				ext[0] = 0;
				anext = (char *)strrchr(load->fileName, '.');
 				ext[0] = '.';
 				ext = anext;
 			}
 			strcpy(szExt, &ext[1]);
 			strlwr(szExt);
 			if (strstr(szExt, "bt")) load->type = GF_SM_LOAD_BT;
			else if (strstr(szExt, "wrl")) load->type = GF_SM_LOAD_VRML;
			else if (strstr(szExt, "x3dv")) load->type = GF_SM_LOAD_X3DV;
#ifndef GPAC_DISABLE_LOADER_XMT
			else if (strstr(szExt, "xmt") || strstr(szExt, "xmta")) load->type = GF_SM_LOAD_XMTA;
			else if (strstr(szExt, "x3d")) load->type = GF_SM_LOAD_X3D;
#endif
			else if (strstr(szExt, "swf")) load->type = GF_SM_LOAD_SWF;
			else if (strstr(szExt, "mov")) load->type = GF_SM_LOAD_QT;
			else if (strstr(szExt, "svg")) load->type = GF_SM_LOAD_SVG;
			else if (strstr(szExt, "xsr")) load->type = GF_SM_LOAD_XSR;
			else if (strstr(szExt, "xbl")) load->type = GF_SM_LOAD_XBL;
			else if (strstr(szExt, "xml")) {
				char *rtype = gf_xml_get_root_type(load->fileName, &e);
				if (rtype) {
					if (!strcmp(rtype, "SAFSession")) load->type = GF_SM_LOAD_XSR;
					else if (!strcmp(rtype, "XMT-A")) load->type = GF_SM_LOAD_XMTA;
					else if (!strcmp(rtype, "X3D")) load->type = GF_SM_LOAD_X3D;
					else if (!strcmp(rtype, "bindings")) load->type = GF_SM_LOAD_XBL;

					gf_free(rtype);
				}
			}
		}
	}
	if (!load->type) return e;

	if (!load->scene_graph) load->scene_graph = load->ctx->scene_graph;

	switch (load->type) {
#ifndef GPAC_DISABLE_LOADER_BT
	case GF_SM_LOAD_BT:
	case GF_SM_LOAD_VRML:
	case GF_SM_LOAD_X3DV:
		return gf_sm_load_init_bt(load);
#endif

#ifndef GPAC_DISABLE_LOADER_XMT
	case GF_SM_LOAD_XMTA:
	case GF_SM_LOAD_X3D:
		return gf_sm_load_init_xmt(load);
#endif

#ifndef GPAC_DISABLE_SVG
	case GF_SM_LOAD_SVG:
	case GF_SM_LOAD_XSR:
	case GF_SM_LOAD_DIMS:
		return gf_sm_load_init_svg(load);

	case GF_SM_LOAD_XBL:
		e = gf_sm_load_init_xbl(load);

		load->process = gf_sm_load_run_xbl;
		load->done = gf_sm_load_done_xbl;
		return e;
#endif

#ifndef GPAC_DISABLE_SWF_IMPORT
	case GF_SM_LOAD_SWF:
		return gf_sm_load_init_swf(load);
#endif

#ifndef GPAC_DISABLE_LOADER_ISOM
	case GF_SM_LOAD_MP4:
		return gf_sm_load_init_isom(load);
#endif

#ifndef GPAC_DISABLE_QTVR
	case GF_SM_LOAD_QT:
		return gf_sm_load_init_qt(load);
#endif
	default:
		return GF_NOT_SUPPORTED;
	}
	return GF_NOT_SUPPORTED;
}
