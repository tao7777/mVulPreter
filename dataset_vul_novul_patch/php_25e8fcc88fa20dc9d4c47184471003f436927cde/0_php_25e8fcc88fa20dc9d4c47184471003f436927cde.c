 static char *ps_files_path_create(char *buf, size_t buflen, ps_files *data, const char *key)
 {
{
	size_t len;
	const char *p;
	char c;
	int ret = 1;

	for (p = key; (c = *p); p++) {
		/* valid characters are a..z,A..Z,0..9 */
		if (!((c >= 'a' && c <= 'z')
				|| (c >= 'A' && c <= 'Z')
				|| (c >= '0' && c <= '9')
				|| c == ','
				|| c == '-')) {
			ret = 0;
			break;
		}
	}

	len = p - key;

	/* Somewhat arbitrary length limit here, but should be way more than
	   anyone needs and avoids file-level warnings later on if we exceed MAX_PATH */
	if (len == 0 || len > 128) {
		ret = 0;
	}

	return ret;
}

static char *ps_files_path_create(char *buf, size_t buflen, ps_files *data, const char *key)
{
	size_t key_len;
	const char *p;
	int i;
	int n;

	key_len = strlen(key);
	if (key_len <= data->dirdepth ||
		buflen < (strlen(data->basedir) + 2 * data->dirdepth + key_len + 5 + sizeof(FILE_PREFIX))) {
		return NULL;
	}

	p = key;
	memcpy(buf, data->basedir, data->basedir_len);
	n = data->basedir_len;
	buf[n++] = PHP_DIR_SEPARATOR;
	for (i = 0; i < (int)data->dirdepth; i++) {
		buf[n++] = *p++;
		buf[n++] = PHP_DIR_SEPARATOR;
	}
	memcpy(buf + n, FILE_PREFIX, sizeof(FILE_PREFIX) - 1);
	n += sizeof(FILE_PREFIX) - 1;
	memcpy(buf + n, key, key_len);
	n += key_len;
 
                ps_files_close(data);
 
               if (php_session_valid_key(key) == FAILURE) {
                        php_error_docref(NULL TSRMLS_CC, E_WARNING, "The session id is too long or contains illegal characters, valid characters are a-z, A-Z, 0-9 and '-,'");
                        return;
                }

                if (!ps_files_path_create(buf, sizeof(buf), data, key)) {
                        return;
                }
	if (data->fd != -1) {
#ifdef PHP_WIN32
		/* On Win32 locked files that are closed without being explicitly unlocked
		   will be unlocked only when "system resources become available". */
		flock(data->fd, LOCK_UN);
#endif
		close(data->fd);
		data->fd = -1;
	}
}

static void ps_files_open(ps_files *data, const char *key TSRMLS_DC)
{
	char buf[MAXPATHLEN];

	if (data->fd < 0 || !data->lastkey || strcmp(key, data->lastkey)) {
		if (data->lastkey) {
			efree(data->lastkey);
			data->lastkey = NULL;
		}

		ps_files_close(data);

		if (!ps_files_valid_key(key)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "The session id is too long or contains illegal characters, valid characters are a-z, A-Z, 0-9 and '-,'");
			PS(invalid_session_id) = 1;
			return;
		}
		if (!ps_files_path_create(buf, sizeof(buf), data, key)) {
			return;
		}

		data->lastkey = estrdup(key);

		data->fd = VCWD_OPEN_MODE(buf, O_CREAT | O_RDWR | O_BINARY, data->filemode);

		if (data->fd != -1) {
#ifndef PHP_WIN32
			/* check to make sure that the opened file is not a symlink, linking to data outside of allowable dirs */
			if (PG(open_basedir)) {
				struct stat sbuf;

				if (fstat(data->fd, &sbuf)) {
					close(data->fd);
					return;
				}
				if (S_ISLNK(sbuf.st_mode) && php_check_open_basedir(buf TSRMLS_CC)) {
					close(data->fd);
					return;
				}
			}
#endif
			flock(data->fd, LOCK_EX);

#ifdef F_SETFD
# ifndef FD_CLOEXEC
#  define FD_CLOEXEC 1
# endif
			if (fcntl(data->fd, F_SETFD, FD_CLOEXEC)) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "fcntl(%d, F_SETFD, FD_CLOEXEC) failed: %s (%d)", data->fd, strerror(errno), errno);
			}
#endif
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "open(%s, O_RDWR) failed: %s (%d)", buf, strerror(errno), errno);
		}
	}
}

static int ps_files_cleanup_dir(const char *dirname, int maxlifetime TSRMLS_DC)
{
	DIR *dir;
	char dentry[sizeof(struct dirent) + MAXPATHLEN];
	struct dirent *entry = (struct dirent *) &dentry;
	struct stat sbuf;
	char buf[MAXPATHLEN];
	time_t now;
	int nrdels = 0;
	size_t dirname_len;

	dir = opendir(dirname);
	if (!dir) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "ps_files_cleanup_dir: opendir(%s) failed: %s (%d)", dirname, strerror(errno), errno);
		return (0);
	}

	time(&now);

        return (nrdels);
 }
 
static int ps_files_key_exists(ps_files *data, const char *key TSRMLS_DC)
{
       char buf[MAXPATHLEN];
       struct stat sbuf;

       if (!key || !ps_files_path_create(buf, sizeof(buf), data, key)) {
               return FAILURE;
       }
       if (VCWD_STAT(buf, &sbuf)) {
               return FAILURE;
       }
       return SUCCESS;
}


 #define PS_FILES_DATA ps_files *data = PS_GET_MOD_DATA()
 
 PS_OPEN_FUNC(files)
						(now - sbuf.st_mtime) > maxlifetime) {
					VCWD_UNLINK(buf);
					nrdels++;
				}
			}
