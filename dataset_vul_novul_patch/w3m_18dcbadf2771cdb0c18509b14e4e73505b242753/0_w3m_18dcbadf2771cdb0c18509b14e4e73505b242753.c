w3m_exit(int i)
{
#ifdef USE_MIGEMO
    init_migemo();		/* close pipe to migemo */
#endif
    stopDownload();
    deleteFiles();
#ifdef USE_SSL
    free_ssl_ctx();
#endif
    disconnectFTP();
#ifdef USE_NNTP
    disconnectNews();
 #endif
 #ifdef __MINGW32_VERSION
     WSACleanup();
#endif
#ifdef HAVE_MKDTEMP
    if (no_rc_dir && tmp_dir != rc_dir)
	if (rmdir(tmp_dir) != 0) {
	    fprintf(stderr, "Can't remove temporary directory (%s)!\n", tmp_dir);
	    exit(1);
	}
 #endif
     exit(i);
 }
