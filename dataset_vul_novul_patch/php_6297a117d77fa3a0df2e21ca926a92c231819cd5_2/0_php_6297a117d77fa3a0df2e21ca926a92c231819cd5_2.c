static int php_stream_memory_stat(php_stream *stream, php_stream_statbuf *ssb TSRMLS_DC) /* {{{ */
{
	time_t timestamp = 0;
	php_stream_memory_data *ms = (php_stream_memory_data*)stream->abstract;
	assert(ms != NULL);
 
        memset(ssb, 0, sizeof(php_stream_statbuf));
        /* read-only across the board */

        ssb->sb.st_mode = ms->mode & TEMP_STREAM_READONLY ? 0444 : 0666;
 
        ssb->sb.st_size = ms->fsize;
	ssb->sb.st_mode |= S_IFREG; /* regular file */

#ifdef NETWARE
	ssb->sb.st_mtime.tv_sec = timestamp;
	ssb->sb.st_atime.tv_sec = timestamp;
	ssb->sb.st_ctime.tv_sec = timestamp;
#else
	ssb->sb.st_mtime = timestamp;
	ssb->sb.st_atime = timestamp;
	ssb->sb.st_ctime = timestamp;
#endif

	ssb->sb.st_nlink = 1;
	ssb->sb.st_rdev = -1;
	/* this is only for APC, so use /dev/null device - no chance of conflict there! */
	ssb->sb.st_dev = 0xC;
	/* generate unique inode number for alias/filename, so no phars will conflict */
	ssb->sb.st_ino = 0;

#ifndef PHP_WIN32
	ssb->sb.st_blksize = -1;
#endif

#if !defined(PHP_WIN32) && !defined(__BEOS__)
	ssb->sb.st_blocks = -1;
#endif

	return 0;
}
/* }}} */
