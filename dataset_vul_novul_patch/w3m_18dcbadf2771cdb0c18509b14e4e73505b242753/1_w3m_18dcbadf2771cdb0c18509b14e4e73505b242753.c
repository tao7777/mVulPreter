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
     exit(i);
 }
