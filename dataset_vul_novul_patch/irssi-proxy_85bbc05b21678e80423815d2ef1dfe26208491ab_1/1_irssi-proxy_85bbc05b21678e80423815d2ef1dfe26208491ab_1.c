static gboolean irssi_ssl_verify(SSL *ssl, SSL_CTX *ctx, X509 *cert)
 {
 	if (SSL_get_verify_result(ssl) != X509_V_OK) {
 		unsigned char md[EVP_MAX_MD_SIZE];
		unsigned int n;
		char *str;

		g_warning("Could not verify SSL servers certificate:");
		if ((str = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0)) == NULL)
			g_warning("  Could not get subject-name from peer certificate");
		else {
			g_warning("  Subject : %s", str);
			free(str);
		}
		if ((str = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0)) == NULL)
			g_warning("  Could not get issuer-name from peer certificate");
		else {
			g_warning("  Issuer  : %s", str);
			free(str);
		}
		if (! X509_digest(cert, EVP_md5(), md, &n))
			g_warning("  Could not get fingerprint from peer certificate");
		else {
			char hex[] = "0123456789ABCDEF";
			char fp[EVP_MAX_MD_SIZE*3];
			if (n < sizeof(fp)) {
				unsigned int i;
				for (i = 0; i < n; i++) {
					fp[i*3+0] = hex[(md[i] >> 4) & 0xF];
					fp[i*3+1] = hex[(md[i] >> 0) & 0xF];
					fp[i*3+2] = i == n - 1 ? '\0' : ':';
				}
				g_warning("  MD5 Fingerprint : %s", fp);
 			}
 		}
 		return FALSE;
 	}
 	return TRUE;
 }
