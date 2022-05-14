_zip_dirent_read(zip_dirent_t *zde, zip_source_t *src, zip_buffer_t *buffer, bool local, zip_error_t *error)
{
    zip_uint8_t buf[CDENTRYSIZE];
    zip_uint16_t dostime, dosdate;
    zip_uint32_t size, variable_size;
    zip_uint16_t filename_len, comment_len, ef_len;

    bool from_buffer = (buffer != NULL);

    size = local ? LENTRYSIZE : CDENTRYSIZE;

    if (buffer) {
        if (_zip_buffer_left(buffer) < size) {
            zip_error_set(error, ZIP_ER_NOZIP, 0);
            return -1;
        }
    }
    else {
        if ((buffer = _zip_buffer_new_from_source(src, size, buf, error)) == NULL) {
            return -1;
        }
    }

    if (memcmp(_zip_buffer_get(buffer, 4), (local ? LOCAL_MAGIC : CENTRAL_MAGIC), 4) != 0) {
	zip_error_set(error, ZIP_ER_NOZIP, 0);
        if (!from_buffer) {
            _zip_buffer_free(buffer);
        }
	return -1;
    }

    /* convert buffercontents to zip_dirent */

    _zip_dirent_init(zde);
    if (!local)
	zde->version_madeby = _zip_buffer_get_16(buffer);
    else
	zde->version_madeby = 0;
    zde->version_needed = _zip_buffer_get_16(buffer);
    zde->bitflags = _zip_buffer_get_16(buffer);
    zde->comp_method = _zip_buffer_get_16(buffer);

    /* convert to time_t */
    dostime = _zip_buffer_get_16(buffer);
    dosdate = _zip_buffer_get_16(buffer);
    zde->last_mod = _zip_d2u_time(dostime, dosdate);

    zde->crc = _zip_buffer_get_32(buffer);
    zde->comp_size = _zip_buffer_get_32(buffer);
    zde->uncomp_size = _zip_buffer_get_32(buffer);

    filename_len = _zip_buffer_get_16(buffer);
    ef_len = _zip_buffer_get_16(buffer);

    if (local) {
	comment_len = 0;
	zde->disk_number = 0;
	zde->int_attrib = 0;
	zde->ext_attrib = 0;
	zde->offset = 0;
    } else {
	comment_len = _zip_buffer_get_16(buffer);
	zde->disk_number = _zip_buffer_get_16(buffer);
	zde->int_attrib = _zip_buffer_get_16(buffer);
	zde->ext_attrib = _zip_buffer_get_32(buffer);
	zde->offset = _zip_buffer_get_32(buffer);
    }

    if (!_zip_buffer_ok(buffer)) {
        zip_error_set(error, ZIP_ER_INTERNAL, 0);
        if (!from_buffer) {
            _zip_buffer_free(buffer);
        }
        return -1;
    }

    if (zde->bitflags & ZIP_GPBF_ENCRYPTED) {
	if (zde->bitflags & ZIP_GPBF_STRONG_ENCRYPTION) {
	    /* TODO */
	    zde->encryption_method = ZIP_EM_UNKNOWN;
	}
	else {
	    zde->encryption_method = ZIP_EM_TRAD_PKWARE;
	}
    }
    else {
	zde->encryption_method = ZIP_EM_NONE;
    }

    zde->filename = NULL;
    zde->extra_fields = NULL;
    zde->comment = NULL;

    variable_size = (zip_uint32_t)filename_len+(zip_uint32_t)ef_len+(zip_uint32_t)comment_len;

    if (from_buffer) {
        if (_zip_buffer_left(buffer) < variable_size) {
            zip_error_set(error, ZIP_ER_INCONS, 0);
            return -1;
        }
    }
    else {
        _zip_buffer_free(buffer);

        if ((buffer = _zip_buffer_new_from_source(src, variable_size, NULL, error)) == NULL) {
            return -1;
        }
    }

    if (filename_len) {
	zde->filename = _zip_read_string(buffer, src, filename_len, 1, error);
        if (!zde->filename) {
            if (zip_error_code_zip(error) == ZIP_ER_EOF) {
                zip_error_set(error, ZIP_ER_INCONS, 0);
            }
            if (!from_buffer) {
                _zip_buffer_free(buffer);
            }
	    return -1;
        }

	if (zde->bitflags & ZIP_GPBF_ENCODING_UTF_8) {
	    if (_zip_guess_encoding(zde->filename, ZIP_ENCODING_UTF8_KNOWN) == ZIP_ENCODING_ERROR) {
		zip_error_set(error, ZIP_ER_INCONS, 0);
                if (!from_buffer) {
                    _zip_buffer_free(buffer);
                }
		return -1;
	    }
	}
    }

    if (ef_len) {
	zip_uint8_t *ef = _zip_read_data(buffer, src, ef_len, 0, error);

        if (ef == NULL) {
            if (!from_buffer) {
                _zip_buffer_free(buffer);
            }
	    return -1;
        }
        if (!_zip_ef_parse(ef, ef_len, local ? ZIP_EF_LOCAL : ZIP_EF_CENTRAL, &zde->extra_fields, error)) {
	    free(ef);
            if (!from_buffer) {
                _zip_buffer_free(buffer);
            }
	    return -1;
	}
	free(ef);
	if (local)
	    zde->local_extra_fields_read = 1;
    }

    if (comment_len) {
	zde->comment = _zip_read_string(buffer, src, comment_len, 0, error);
        if (!zde->comment) {
            if (!from_buffer) {
                _zip_buffer_free(buffer);
            }
	    return -1;
        }
	if (zde->bitflags & ZIP_GPBF_ENCODING_UTF_8) {
	    if (_zip_guess_encoding(zde->comment, ZIP_ENCODING_UTF8_KNOWN) == ZIP_ENCODING_ERROR) {
		zip_error_set(error, ZIP_ER_INCONS, 0);
                if (!from_buffer) {
                    _zip_buffer_free(buffer);
                }
		return -1;
	    }
	}
    }

    zde->filename = _zip_dirent_process_ef_utf_8(zde, ZIP_EF_UTF_8_NAME, zde->filename);
    zde->comment = _zip_dirent_process_ef_utf_8(zde, ZIP_EF_UTF_8_COMMENT, zde->comment);

    /* Zip64 */

    if (zde->uncomp_size == ZIP_UINT32_MAX || zde->comp_size == ZIP_UINT32_MAX || zde->offset == ZIP_UINT32_MAX) {
	zip_uint16_t got_len;
        zip_buffer_t *ef_buffer;
	const zip_uint8_t *ef = _zip_ef_get_by_id(zde->extra_fields, &got_len, ZIP_EF_ZIP64, 0, local ? ZIP_EF_LOCAL : ZIP_EF_CENTRAL, error);
	/* TODO: if got_len == 0 && !ZIP64_EOCD: no error, 0xffffffff is valid value */
        if (ef == NULL) {
            if (!from_buffer) {
                _zip_buffer_free(buffer);
            }
	    return -1;
        }

        if ((ef_buffer = _zip_buffer_new((zip_uint8_t *)ef, got_len)) == NULL) {
            zip_error_set(error, ZIP_ER_MEMORY, 0);
            if (!from_buffer) {
                _zip_buffer_free(buffer);
            }
            return -1;
        }

	if (zde->uncomp_size == ZIP_UINT32_MAX)
	    zde->uncomp_size = _zip_buffer_get_64(ef_buffer);
	else if (local) {
	    /* From appnote.txt: This entry in the Local header MUST
	       include BOTH original and compressed file size fields. */
            (void)_zip_buffer_skip(ef_buffer, 8); /* error is caught by _zip_buffer_eof() call */
	}
	if (zde->comp_size == ZIP_UINT32_MAX)
	    zde->comp_size = _zip_buffer_get_64(ef_buffer);
	if (!local) {
	    if (zde->offset == ZIP_UINT32_MAX)
		zde->offset = _zip_buffer_get_64(ef_buffer);
	    if (zde->disk_number == ZIP_UINT16_MAX)
		zde->disk_number = _zip_buffer_get_32(buffer);
	}

        if (!_zip_buffer_eof(ef_buffer)) {
            zip_error_set(error, ZIP_ER_INCONS, 0);
            _zip_buffer_free(ef_buffer);
            if (!from_buffer) {
                _zip_buffer_free(buffer);
            }
            return -1;
        }
        _zip_buffer_free(ef_buffer);
    }

    if (!_zip_buffer_ok(buffer)) {
        zip_error_set(error, ZIP_ER_INTERNAL, 0);
        if (!from_buffer) {
            _zip_buffer_free(buffer);
        }
        return -1;
    }
    if (!from_buffer) {
        _zip_buffer_free(buffer);
    }

    /* zip_source_seek / zip_source_tell don't support values > ZIP_INT64_MAX */
    if (zde->offset > ZIP_INT64_MAX) {
	zip_error_set(error, ZIP_ER_SEEK, EFBIG);
	return -1;
     }
 
     if (!_zip_dirent_process_winzip_aes(zde, error)) {
 	return -1;
     }
 
    zde->extra_fields = _zip_ef_remove_internal(zde->extra_fields);

    return (zip_int64_t)(size + variable_size);
}
