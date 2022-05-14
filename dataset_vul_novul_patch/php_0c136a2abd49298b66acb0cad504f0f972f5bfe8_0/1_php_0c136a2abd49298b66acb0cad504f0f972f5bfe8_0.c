int make_http_soap_request(zval  *this_ptr,
                           char  *buf,
                           int    buf_size,
                           char  *location,
                           char  *soapaction,
                           int    soap_version,
                           char **buffer,
                           int   *buffer_len TSRMLS_DC)
{
	char *request;
	smart_str soap_headers = {0};
	smart_str soap_headers_z = {0};
	int request_size, err;
	php_url *phpurl = NULL;
	php_stream *stream;
	zval **trace, **tmp;
	int use_proxy = 0;
	int use_ssl;
	char *http_headers, *http_body, *content_type, *http_version, *cookie_itt;
	int http_header_size, http_body_size, http_close;
	char *connection;
	int http_1_1;
	int http_status;
	int content_type_xml = 0;
	long redirect_max = 20;
	char *content_encoding;
	char *http_msg = NULL;
	zend_bool old_allow_url_fopen;
	php_stream_context *context = NULL;
	zend_bool has_authorization = 0;
	zend_bool has_proxy_authorization = 0;
	zend_bool has_cookies = 0;

	if (this_ptr == NULL || Z_TYPE_P(this_ptr) != IS_OBJECT) {
		return FALSE;
	}

  request = buf;
  request_size = buf_size;
	/* Compress request */
	if (zend_hash_find(Z_OBJPROP_P(this_ptr), "compression", sizeof("compression"), (void **)&tmp) == SUCCESS && Z_TYPE_PP(tmp) == IS_LONG) {
		int level = Z_LVAL_PP(tmp) & 0x0f;
		int kind  = Z_LVAL_PP(tmp) & SOAP_COMPRESSION_DEFLATE;

		if (level > 9) {level = 9;}
		
	  if ((Z_LVAL_PP(tmp) & SOAP_COMPRESSION_ACCEPT) != 0) {
			smart_str_append_const(&soap_headers_z,"Accept-Encoding: gzip, deflate\r\n");
	  }
	  if (level > 0) {
			zval func;
			zval retval;
 			zval param1, param2, param3;
			zval *params[3];
			int n;

			params[0] = &param1;
			INIT_PZVAL(params[0]);
			params[1] = &param2;
			INIT_PZVAL(params[1]);
			params[2] = &param3;
			INIT_PZVAL(params[2]);
			ZVAL_STRINGL(params[0], buf, buf_size, 0);
			ZVAL_LONG(params[1], level);
	    if (kind == SOAP_COMPRESSION_DEFLATE) {
	    	n = 2;
				ZVAL_STRING(&func, "gzcompress", 0);
				smart_str_append_const(&soap_headers_z,"Content-Encoding: deflate\r\n");
	    } else {
	      n = 3;
				ZVAL_STRING(&func, "gzencode", 0);
				smart_str_append_const(&soap_headers_z,"Content-Encoding: gzip\r\n");
				ZVAL_LONG(params[2], 0x1f);
	    }
			if (call_user_function(CG(function_table), (zval**)NULL, &func, &retval, n, params TSRMLS_CC) == SUCCESS &&
			    Z_TYPE(retval) == IS_STRING) {
				request = Z_STRVAL(retval);
				request_size = Z_STRLEN(retval);
			} else {
				if (request != buf) {efree(request);}
				smart_str_free(&soap_headers_z);
				return FALSE;
			}
	  }
	}

	if (zend_hash_find(Z_OBJPROP_P(this_ptr), "httpsocket", sizeof("httpsocket"), (void **)&tmp) == SUCCESS) {
		php_stream_from_zval_no_verify(stream,tmp);
		if (zend_hash_find(Z_OBJPROP_P(this_ptr), "_use_proxy", sizeof("_use_proxy"), (void **)&tmp) == SUCCESS && Z_TYPE_PP(tmp) == IS_LONG) {
			use_proxy = Z_LVAL_PP(tmp);
		}
	} else {
		stream = NULL;
	}

	if (location != NULL && location[0] != '\000') {
		phpurl = php_url_parse(location);
	}

	if (SUCCESS == zend_hash_find(Z_OBJPROP_P(this_ptr),
			"_stream_context", sizeof("_stream_context"), (void**)&tmp)) {
		context = php_stream_context_from_zval(*tmp, 0);
	}

	if (context && 
		php_stream_context_get_option(context, "http", "max_redirects", &tmp) == SUCCESS) {
		if (Z_TYPE_PP(tmp) != IS_STRING || !is_numeric_string(Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp), &redirect_max, NULL, 1)) {
			if (Z_TYPE_PP(tmp) == IS_LONG)
				redirect_max = Z_LVAL_PP(tmp);
		}
	}

try_again:
	if (phpurl == NULL || phpurl->host == NULL) {
	  if (phpurl != NULL) {php_url_free(phpurl);}
		if (request != buf) {efree(request);}
		add_soap_fault(this_ptr, "HTTP", "Unable to parse URL", NULL, NULL TSRMLS_CC);
		smart_str_free(&soap_headers_z);
		return FALSE;
	}

	use_ssl = 0;
	if (phpurl->scheme != NULL && strcmp(phpurl->scheme, "https") == 0) {
		use_ssl = 1;
	} else if (phpurl->scheme == NULL || strcmp(phpurl->scheme, "http") != 0) {
		php_url_free(phpurl);
		if (request != buf) {efree(request);}
		add_soap_fault(this_ptr, "HTTP", "Unknown protocol. Only http and https are allowed.", NULL, NULL TSRMLS_CC);
		smart_str_free(&soap_headers_z);
		return FALSE;
	}

	old_allow_url_fopen = PG(allow_url_fopen);
	PG(allow_url_fopen) = 1;
	if (use_ssl && php_stream_locate_url_wrapper("https://", NULL, STREAM_LOCATE_WRAPPERS_ONLY TSRMLS_CC) == NULL) {
		php_url_free(phpurl);
		if (request != buf) {efree(request);}
		add_soap_fault(this_ptr, "HTTP", "SSL support is not available in this build", NULL, NULL TSRMLS_CC);
		PG(allow_url_fopen) = old_allow_url_fopen;
		smart_str_free(&soap_headers_z);
		return FALSE;
	}

	if (phpurl->port == 0) {
		phpurl->port = use_ssl ? 443 : 80;
	}

	/* Check if request to the same host */
	if (stream != NULL) {
	  php_url *orig;
		if (zend_hash_find(Z_OBJPROP_P(this_ptr), "httpurl", sizeof("httpurl"), (void **)&tmp) == SUCCESS &&
		    (orig = (php_url *) zend_fetch_resource(tmp TSRMLS_CC, -1, "httpurl", NULL, 1, le_url)) != NULL &&
		    ((use_proxy && !use_ssl) ||
		     (((use_ssl && orig->scheme != NULL && strcmp(orig->scheme, "https") == 0) ||
		      (!use_ssl && orig->scheme == NULL) ||
		      (!use_ssl && strcmp(orig->scheme, "https") != 0)) &&
		     strcmp(orig->host, phpurl->host) == 0 &&
		     orig->port == phpurl->port))) {
    } else {
			php_stream_close(stream);
			zend_hash_del(Z_OBJPROP_P(this_ptr), "httpurl", sizeof("httpurl"));
			zend_hash_del(Z_OBJPROP_P(this_ptr), "httpsocket", sizeof("httpsocket"));
			zend_hash_del(Z_OBJPROP_P(this_ptr), "_use_proxy", sizeof("_use_proxy"));
			stream = NULL;
			use_proxy = 0;
    }
	}

	/* Check if keep-alive connection is still opened */
	if (stream != NULL && php_stream_eof(stream)) {
		php_stream_close(stream);
		zend_hash_del(Z_OBJPROP_P(this_ptr), "httpurl", sizeof("httpurl"));
		zend_hash_del(Z_OBJPROP_P(this_ptr), "httpsocket", sizeof("httpsocket"));
		zend_hash_del(Z_OBJPROP_P(this_ptr), "_use_proxy", sizeof("_use_proxy"));
		stream = NULL;
		use_proxy = 0;
	}

	if (!stream) {
		stream = http_connect(this_ptr, phpurl, use_ssl, context, &use_proxy TSRMLS_CC);
		if (stream) {
			php_stream_auto_cleanup(stream);
			add_property_resource(this_ptr, "httpsocket", php_stream_get_resource_id(stream));
			add_property_long(this_ptr, "_use_proxy", use_proxy);
		} else {
			php_url_free(phpurl);
			if (request != buf) {efree(request);}
			add_soap_fault(this_ptr, "HTTP", "Could not connect to host", NULL, NULL TSRMLS_CC);
			PG(allow_url_fopen) = old_allow_url_fopen;
			smart_str_free(&soap_headers_z);
			return FALSE;
		}
	}
	PG(allow_url_fopen) = old_allow_url_fopen;

	if (stream) {
		zval **cookies, **login, **password;
	  int ret = zend_list_insert(phpurl, le_url TSRMLS_CC);

		add_property_resource(this_ptr, "httpurl", ret);
		/*zend_list_addref(ret);*/

		if (context && 
		    php_stream_context_get_option(context, "http", "protocol_version", &tmp) == SUCCESS &&
		    Z_TYPE_PP(tmp) == IS_DOUBLE &&
		    Z_DVAL_PP(tmp) == 1.0) {
			http_1_1 = 0;
		} else {
			http_1_1 = 1;
		}

		smart_str_append_const(&soap_headers, "POST ");
		if (use_proxy && !use_ssl) {
			smart_str_appends(&soap_headers, phpurl->scheme);
			smart_str_append_const(&soap_headers, "://");
			smart_str_appends(&soap_headers, phpurl->host);
			smart_str_appendc(&soap_headers, ':');
			smart_str_append_unsigned(&soap_headers, phpurl->port);
		}
		if (phpurl->path) {
			smart_str_appends(&soap_headers, phpurl->path);
		} else {
			smart_str_appendc(&soap_headers, '/');
		}
		if (phpurl->query) {
			smart_str_appendc(&soap_headers, '?');
			smart_str_appends(&soap_headers, phpurl->query);
		}
		if (phpurl->fragment) {
			smart_str_appendc(&soap_headers, '#');
			smart_str_appends(&soap_headers, phpurl->fragment);
		}
		if (http_1_1) {
			smart_str_append_const(&soap_headers, " HTTP/1.1\r\n");
		} else {
			smart_str_append_const(&soap_headers, " HTTP/1.0\r\n");
		}
		smart_str_append_const(&soap_headers, "Host: ");
		smart_str_appends(&soap_headers, phpurl->host);
		if (phpurl->port != (use_ssl?443:80)) {
			smart_str_appendc(&soap_headers, ':');
			smart_str_append_unsigned(&soap_headers, phpurl->port);
		}
		if (!http_1_1 ||
			(zend_hash_find(Z_OBJPROP_P(this_ptr), "_keep_alive", sizeof("_keep_alive"), (void **)&tmp) == SUCCESS &&
			 Z_LVAL_PP(tmp) == 0)) {
                }
                if (!http_1_1 ||
                        (zend_hash_find(Z_OBJPROP_P(this_ptr), "_keep_alive", sizeof("_keep_alive"), (void **)&tmp) == SUCCESS &&
                         Z_LVAL_PP(tmp) == 0)) {
                        smart_str_append_const(&soap_headers, "\r\n"
                                "Connection: close\r\n");
		    Z_TYPE_PP(tmp) == IS_STRING) {
			if (Z_STRLEN_PP(tmp) > 0) {
				smart_str_append_const(&soap_headers, "User-Agent: ");
				smart_str_appendl(&soap_headers, Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp));
				smart_str_append_const(&soap_headers, "\r\n");
			}
		} else if (context && 
		           php_stream_context_get_option(context, "http", "user_agent", &tmp) == SUCCESS &&
		           Z_TYPE_PP(tmp) == IS_STRING) {
			if (Z_STRLEN_PP(tmp) > 0) {
				smart_str_append_const(&soap_headers, "User-Agent: ");
				smart_str_appendl(&soap_headers, Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp));
				smart_str_append_const(&soap_headers, "\r\n");
			}
		} else if (FG(user_agent)) {
			smart_str_append_const(&soap_headers, "User-Agent: ");
			smart_str_appends(&soap_headers, FG(user_agent));
			smart_str_append_const(&soap_headers, "\r\n");
		} else {
			smart_str_append_const(&soap_headers, "User-Agent: PHP-SOAP/"PHP_VERSION"\r\n");
		}

		smart_str_append(&soap_headers, &soap_headers_z);

		if (soap_version == SOAP_1_2) {
			smart_str_append_const(&soap_headers,"Content-Type: application/soap+xml; charset=utf-8");
			if (soapaction) {
				smart_str_append_const(&soap_headers,"; action=\"");
				smart_str_appends(&soap_headers, soapaction);
				smart_str_append_const(&soap_headers,"\"");
			}
			smart_str_append_const(&soap_headers,"\r\n");
		} else {
			smart_str_append_const(&soap_headers,"Content-Type: text/xml; charset=utf-8\r\n");
			if (soapaction) {
				smart_str_append_const(&soap_headers, "SOAPAction: \"");
				smart_str_appends(&soap_headers, soapaction);
				smart_str_append_const(&soap_headers, "\"\r\n");
			}
		}
		smart_str_append_const(&soap_headers,"Content-Length: ");
		smart_str_append_long(&soap_headers, request_size);
		smart_str_append_const(&soap_headers, "\r\n");

		/* HTTP Authentication */
		if (zend_hash_find(Z_OBJPROP_P(this_ptr), "_login", sizeof("_login"), (void **)&login) == SUCCESS &&
		    Z_TYPE_PP(login) == IS_STRING) {
			zval **digest;

			has_authorization = 1;
			if (zend_hash_find(Z_OBJPROP_P(this_ptr), "_digest", sizeof("_digest"), (void **)&digest) == SUCCESS) {
				if (Z_TYPE_PP(digest) == IS_ARRAY) {
					char          HA1[33], HA2[33], response[33], cnonce[33], nc[9];
					PHP_MD5_CTX   md5ctx;
					unsigned char hash[16];

					PHP_MD5Init(&md5ctx);
					snprintf(cnonce, sizeof(cnonce), "%ld", php_rand(TSRMLS_C));
					PHP_MD5Update(&md5ctx, (unsigned char*)cnonce, strlen(cnonce));
					PHP_MD5Final(hash, &md5ctx);
					make_digest(cnonce, hash);

					if (zend_hash_find(Z_ARRVAL_PP(digest), "nc", sizeof("nc"), (void **)&tmp) == SUCCESS &&
					    Z_TYPE_PP(tmp) == IS_LONG) {
						Z_LVAL_PP(tmp)++;
						snprintf(nc, sizeof(nc), "%08ld", Z_LVAL_PP(tmp));
					} else {
						add_assoc_long(*digest, "nc", 1);
						strcpy(nc, "00000001");
					}

					PHP_MD5Init(&md5ctx);
					PHP_MD5Update(&md5ctx, (unsigned char*)Z_STRVAL_PP(login), Z_STRLEN_PP(login));
					PHP_MD5Update(&md5ctx, (unsigned char*)":", 1);
					if (zend_hash_find(Z_ARRVAL_PP(digest), "realm", sizeof("realm"), (void **)&tmp) == SUCCESS &&
					    Z_TYPE_PP(tmp) == IS_STRING) {
						PHP_MD5Update(&md5ctx, (unsigned char*)Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp));
					}
					PHP_MD5Update(&md5ctx, (unsigned char*)":", 1);
					if (zend_hash_find(Z_OBJPROP_P(this_ptr), "_password", sizeof("_password"), (void **)&password) == SUCCESS &&
					    Z_TYPE_PP(password) == IS_STRING) {
						PHP_MD5Update(&md5ctx, (unsigned char*)Z_STRVAL_PP(password), Z_STRLEN_PP(password));
					}
					PHP_MD5Final(hash, &md5ctx);
					make_digest(HA1, hash);
					if (zend_hash_find(Z_ARRVAL_PP(digest), "algorithm", sizeof("algorithm"), (void **)&tmp) == SUCCESS &&
					    Z_TYPE_PP(tmp) == IS_STRING &&
					    Z_STRLEN_PP(tmp) == sizeof("md5-sess")-1 &&
					    stricmp(Z_STRVAL_PP(tmp), "md5-sess") == 0) {
						PHP_MD5Init(&md5ctx);
						PHP_MD5Update(&md5ctx, (unsigned char*)HA1, 32);
						PHP_MD5Update(&md5ctx, (unsigned char*)":", 1);
						if (zend_hash_find(Z_ARRVAL_PP(digest), "nonce", sizeof("nonce"), (void **)&tmp) == SUCCESS &&
						    Z_TYPE_PP(tmp) == IS_STRING) {
							PHP_MD5Update(&md5ctx, (unsigned char*)Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp));
						}
						PHP_MD5Update(&md5ctx, (unsigned char*)":", 1);
						PHP_MD5Update(&md5ctx, (unsigned char*)cnonce, 8);
						PHP_MD5Final(hash, &md5ctx);
						make_digest(HA1, hash);
					}

					PHP_MD5Init(&md5ctx);
					PHP_MD5Update(&md5ctx, (unsigned char*)"POST:", sizeof("POST:")-1);
					if (phpurl->path) {
						PHP_MD5Update(&md5ctx, (unsigned char*)phpurl->path, strlen(phpurl->path));
					} else {
						PHP_MD5Update(&md5ctx, (unsigned char*)"/", 1);
					}
					if (phpurl->query) {
						PHP_MD5Update(&md5ctx, (unsigned char*)"?", 1);
						PHP_MD5Update(&md5ctx, (unsigned char*)phpurl->query, strlen(phpurl->query));
					}

					/* TODO: Support for qop="auth-int" */
/*
					if (zend_hash_find(Z_ARRVAL_PP(digest), "qop", sizeof("qop"), (void **)&tmp) == SUCCESS &&
					    Z_TYPE_PP(tmp) == IS_STRING &&
					    Z_STRLEN_PP(tmp) == sizeof("auth-int")-1 &&
					    stricmp(Z_STRVAL_PP(tmp), "auth-int") == 0) {
						PHP_MD5Update(&md5ctx, ":", 1);
						PHP_MD5Update(&md5ctx, HEntity, HASHHEXLEN);
					}
*/
					PHP_MD5Final(hash, &md5ctx);
					make_digest(HA2, hash);

					PHP_MD5Init(&md5ctx);
					PHP_MD5Update(&md5ctx, (unsigned char*)HA1, 32);
					PHP_MD5Update(&md5ctx, (unsigned char*)":", 1);
					if (zend_hash_find(Z_ARRVAL_PP(digest), "nonce", sizeof("nonce"), (void **)&tmp) == SUCCESS &&
					    Z_TYPE_PP(tmp) == IS_STRING) {
						PHP_MD5Update(&md5ctx, (unsigned char*)Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp));
					}
					PHP_MD5Update(&md5ctx, (unsigned char*)":", 1);
					if (zend_hash_find(Z_ARRVAL_PP(digest), "qop", sizeof("qop"), (void **)&tmp) == SUCCESS &&
					    Z_TYPE_PP(tmp) == IS_STRING) {
						PHP_MD5Update(&md5ctx, (unsigned char*)nc, 8);
						PHP_MD5Update(&md5ctx, (unsigned char*)":", 1);
						PHP_MD5Update(&md5ctx, (unsigned char*)cnonce, 8);
						PHP_MD5Update(&md5ctx, (unsigned char*)":", 1);
						/* TODO: Support for qop="auth-int" */
						PHP_MD5Update(&md5ctx, (unsigned char*)"auth", sizeof("auth")-1);
						PHP_MD5Update(&md5ctx, (unsigned char*)":", 1);
					}
					PHP_MD5Update(&md5ctx, (unsigned char*)HA2, 32);
					PHP_MD5Final(hash, &md5ctx);
					make_digest(response, hash);
	
					smart_str_append_const(&soap_headers, "Authorization: Digest username=\"");
					smart_str_appendl(&soap_headers, Z_STRVAL_PP(login), Z_STRLEN_PP(login));
					if (zend_hash_find(Z_ARRVAL_PP(digest), "realm", sizeof("realm"), (void **)&tmp) == SUCCESS &&
					    Z_TYPE_PP(tmp) == IS_STRING) {
						smart_str_append_const(&soap_headers, "\", realm=\"");
						smart_str_appendl(&soap_headers, Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp));
					}
					if (zend_hash_find(Z_ARRVAL_PP(digest), "nonce", sizeof("nonce"), (void **)&tmp) == SUCCESS &&
					    Z_TYPE_PP(tmp) == IS_STRING) {
						smart_str_append_const(&soap_headers, "\", nonce=\"");
						smart_str_appendl(&soap_headers, Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp));
					}
					smart_str_append_const(&soap_headers, "\", uri=\"");
					if (phpurl->path) {
						smart_str_appends(&soap_headers, phpurl->path);
					} else {
						smart_str_appendc(&soap_headers, '/');
					} 
					if (phpurl->query) {
						smart_str_appendc(&soap_headers, '?');
						smart_str_appends(&soap_headers, phpurl->query);
					}
					if (phpurl->fragment) {
						smart_str_appendc(&soap_headers, '#');
						smart_str_appends(&soap_headers, phpurl->fragment);
					}
					if (zend_hash_find(Z_ARRVAL_PP(digest), "qop", sizeof("qop"), (void **)&tmp) == SUCCESS &&
					    Z_TYPE_PP(tmp) == IS_STRING) {
					/* TODO: Support for qop="auth-int" */
						smart_str_append_const(&soap_headers, "\", qop=\"auth");
						smart_str_append_const(&soap_headers, "\", nc=\"");
						smart_str_appendl(&soap_headers, nc, 8);
						smart_str_append_const(&soap_headers, "\", cnonce=\"");
						smart_str_appendl(&soap_headers, cnonce, 8);
					}
					smart_str_append_const(&soap_headers, "\", response=\"");
					smart_str_appendl(&soap_headers, response, 32);
					if (zend_hash_find(Z_ARRVAL_PP(digest), "opaque", sizeof("opaque"), (void **)&tmp) == SUCCESS &&
					    Z_TYPE_PP(tmp) == IS_STRING) {
						smart_str_append_const(&soap_headers, "\", opaque=\"");
						smart_str_appendl(&soap_headers, Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp));
					}
					if (zend_hash_find(Z_ARRVAL_PP(digest), "algorithm", sizeof("algorithm"), (void **)&tmp) == SUCCESS &&
						Z_TYPE_PP(tmp) == IS_STRING) {
						smart_str_append_const(&soap_headers, "\", algorithm=\"");
						smart_str_appendl(&soap_headers, Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp));
					}
					smart_str_append_const(&soap_headers, "\"\r\n");
				}
			} else {
				unsigned char* buf;
				int len;

				smart_str auth = {0};
				smart_str_appendl(&auth, Z_STRVAL_PP(login), Z_STRLEN_PP(login));
				smart_str_appendc(&auth, ':');
				if (zend_hash_find(Z_OBJPROP_P(this_ptr), "_password", sizeof("_password"), (void **)&password) == SUCCESS &&
				    Z_TYPE_PP(password) == IS_STRING) {
					smart_str_appendl(&auth, Z_STRVAL_PP(password), Z_STRLEN_PP(password));
				}
				smart_str_0(&auth);
				buf = php_base64_encode((unsigned char*)auth.c, auth.len, &len);
				smart_str_append_const(&soap_headers, "Authorization: Basic ");
				smart_str_appendl(&soap_headers, (char*)buf, len);
				smart_str_append_const(&soap_headers, "\r\n");
				efree(buf);
				smart_str_free(&auth);
			}
		}

		/* Proxy HTTP Authentication */
		if (use_proxy && !use_ssl) {
			has_proxy_authorization = proxy_authentication(this_ptr, &soap_headers TSRMLS_CC);
		}

		/* Send cookies along with request */
		if (zend_hash_find(Z_OBJPROP_P(this_ptr), "_cookies", sizeof("_cookies"), (void **)&cookies) == SUCCESS) {
			zval **data;
                }
 
                /* Send cookies along with request */
               if (zend_hash_find(Z_OBJPROP_P(this_ptr), "_cookies", sizeof("_cookies"), (void **)&cookies) == SUCCESS) {
                        zval **data;
                        char *key;
                        int i, n;
				for (i = 0; i < n; i++) {
					zend_hash_get_current_data(Z_ARRVAL_PP(cookies), (void **)&data);
					zend_hash_get_current_key(Z_ARRVAL_PP(cookies), &key, NULL, FALSE);

					if (Z_TYPE_PP(data) == IS_ARRAY) {
					  zval** value;

						if (zend_hash_index_find(Z_ARRVAL_PP(data), 0, (void**)&value) == SUCCESS &&
						    Z_TYPE_PP(value) == IS_STRING) {
						  zval **tmp;
						  if ((zend_hash_index_find(Z_ARRVAL_PP(data), 1, (void**)&tmp) == FAILURE ||
						       strncmp(phpurl->path?phpurl->path:"/",Z_STRVAL_PP(tmp),Z_STRLEN_PP(tmp)) == 0) &&
						      (zend_hash_index_find(Z_ARRVAL_PP(data), 2, (void**)&tmp) == FAILURE ||
						       in_domain(phpurl->host,Z_STRVAL_PP(tmp))) &&
						      (use_ssl || zend_hash_index_find(Z_ARRVAL_PP(data), 3, (void**)&tmp) == FAILURE)) {
								smart_str_appendl(&soap_headers, key, strlen(key));
								smart_str_appendc(&soap_headers, '=');
								smart_str_appendl(&soap_headers, Z_STRVAL_PP(value), Z_STRLEN_PP(value));
								smart_str_appendc(&soap_headers, ';');
							}
						}
					}
					zend_hash_move_forward(Z_ARRVAL_PP(cookies));
				}
				smart_str_append_const(&soap_headers, "\r\n");
			}
		}

		http_context_headers(context, has_authorization, has_proxy_authorization, has_cookies, &soap_headers TSRMLS_CC);

		smart_str_append_const(&soap_headers, "\r\n");
		smart_str_0(&soap_headers);
		if (zend_hash_find(Z_OBJPROP_P(this_ptr), "trace", sizeof("trace"), (void **) &trace) == SUCCESS &&
		    Z_LVAL_PP(trace) > 0) {
			add_property_stringl(this_ptr, "__last_request_headers", soap_headers.c, soap_headers.len, 1);
		}
                smart_str_append_const(&soap_headers, "\r\n");
                smart_str_0(&soap_headers);
                if (zend_hash_find(Z_OBJPROP_P(this_ptr), "trace", sizeof("trace"), (void **) &trace) == SUCCESS &&
                   Z_LVAL_PP(trace) > 0) {
                        add_property_stringl(this_ptr, "__last_request_headers", soap_headers.c, soap_headers.len, 1);
                }
                smart_str_appendl(&soap_headers, request, request_size);
			zend_hash_del(Z_OBJPROP_P(this_ptr), "httpurl", sizeof("httpurl"));
			zend_hash_del(Z_OBJPROP_P(this_ptr), "httpsocket", sizeof("httpsocket"));
			zend_hash_del(Z_OBJPROP_P(this_ptr), "_use_proxy", sizeof("_use_proxy"));
			add_soap_fault(this_ptr, "HTTP", "Failed Sending HTTP SOAP request", NULL, NULL TSRMLS_CC);
			smart_str_free(&soap_headers_z);
			return FALSE;
		}
		smart_str_free(&soap_headers);
	} else {
		add_soap_fault(this_ptr, "HTTP", "Failed to create stream??", NULL, NULL TSRMLS_CC);
		smart_str_free(&soap_headers_z);
		return FALSE;
	}
