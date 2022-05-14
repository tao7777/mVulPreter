int CMS_verify(CMS_ContentInfo *cms, STACK_OF(X509) *certs,
		 X509_STORE *store, BIO *dcont, BIO *out, unsigned int flags)
	{
	CMS_SignerInfo *si;
	STACK_OF(CMS_SignerInfo) *sinfos;
	STACK_OF(X509) *cms_certs = NULL;
 	STACK_OF(X509_CRL) *crls = NULL;
 	X509 *signer;
 	int i, scount = 0, ret = 0;
	BIO *cmsbio = NULL, *tmpin = NULL, *tmpout = NULL;
 
 	if (!dcont && !check_content(cms))
 		return 0;

	/* Attempt to find all signer certificates */

	sinfos = CMS_get0_SignerInfos(cms);

	if (sk_CMS_SignerInfo_num(sinfos) <= 0)
		{
		CMSerr(CMS_F_CMS_VERIFY, CMS_R_NO_SIGNERS);
		goto err;
		}

	for (i = 0; i < sk_CMS_SignerInfo_num(sinfos); i++)
		{
		si = sk_CMS_SignerInfo_value(sinfos, i);
		CMS_SignerInfo_get0_algs(si, NULL, &signer, NULL, NULL);
		if (signer)
			scount++;
		}

	if (scount != sk_CMS_SignerInfo_num(sinfos))
		scount += CMS_set1_signers_certs(cms, certs, flags);

	if (scount != sk_CMS_SignerInfo_num(sinfos))
		{
		CMSerr(CMS_F_CMS_VERIFY, CMS_R_SIGNER_CERTIFICATE_NOT_FOUND);
		goto err;
		}

	/* Attempt to verify all signers certs */

	if (!(flags & CMS_NO_SIGNER_CERT_VERIFY))
		{
		cms_certs = CMS_get1_certs(cms);
		if (!(flags & CMS_NOCRL))
			crls = CMS_get1_crls(cms);
		for (i = 0; i < sk_CMS_SignerInfo_num(sinfos); i++)
			{
			si = sk_CMS_SignerInfo_value(sinfos, i);
			if (!cms_signerinfo_verify_cert(si, store,
							cms_certs, crls, flags))
				goto err;
			}
		}

	/* Attempt to verify all SignerInfo signed attribute signatures */

	if (!(flags & CMS_NO_ATTR_VERIFY))
		{
		for (i = 0; i < sk_CMS_SignerInfo_num(sinfos); i++)
			{
			si = sk_CMS_SignerInfo_value(sinfos, i);
			if (CMS_signed_get_attr_count(si) < 0)
				continue;
			if (CMS_SignerInfo_verify(si) <= 0)
				goto err;
			}
		}

	/* Performance optimization: if the content is a memory BIO then
	 * store its contents in a temporary read only memory BIO. This
	 * avoids potentially large numbers of slow copies of data which will
	 * occur when reading from a read write memory BIO when signatures
	 * are calculated.
	 */

	if (dcont && (BIO_method_type(dcont) == BIO_TYPE_MEM))
		{
		char *ptr;
		long len;
		len = BIO_get_mem_data(dcont, &ptr);
		tmpin = BIO_new_mem_buf(ptr, len);
		if (tmpin == NULL)
			{
			CMSerr(CMS_F_CMS_VERIFY,ERR_R_MALLOC_FAILURE);
			return 0;
			}
 		}
 	else
 		tmpin = dcont;
	/* If not binary mode and detached generate digests by *writing*
	 * through the BIO. That makes it possible to canonicalise the
	 * input.
	 */
	if (!(flags & SMIME_BINARY) && dcont)
		{
		/* Create output BIO so we can either handle text or to
		 * ensure included content doesn't override detached content.
		 */
		tmpout = cms_get_text_bio(out, flags);
		if(!tmpout)
			{
			CMSerr(CMS_F_CMS_VERIFY,ERR_R_MALLOC_FAILURE);
			goto err;
			}
		cmsbio = CMS_dataInit(cms, tmpout);
		if (!cmsbio)
			goto err;
		/* Don't use SMIME_TEXT for verify: it adds headers and
		 * we want to remove them.
		 */
		SMIME_crlf_copy(dcont, cmsbio, flags & ~SMIME_TEXT);
 
		if(flags & CMS_TEXT)
			{
			if (!SMIME_text(tmpout, out))
				{
				CMSerr(CMS_F_CMS_VERIFY,CMS_R_SMIME_TEXT_ERROR);
				goto err;
				}
			}
		}
	else
		{
		cmsbio=CMS_dataInit(cms, tmpin);
		if (!cmsbio)
			goto err;
 
		if (!cms_copy_content(out, cmsbio, flags))
			goto err;
 
		}
 	if (!(flags & CMS_NO_CONTENT_VERIFY))
 		{
 		for (i = 0; i < sk_CMS_SignerInfo_num(sinfos); i++)
			{
			si = sk_CMS_SignerInfo_value(sinfos, i);
			if (CMS_SignerInfo_verify_content(si, cmsbio) <= 0)
				{
				CMSerr(CMS_F_CMS_VERIFY,
					CMS_R_CONTENT_VERIFY_ERROR);
				goto err;
				}
			}
		}

 	ret = 1;
 
 	err:
	if (!(flags & SMIME_BINARY) && dcont)
		{
		do_free_upto(cmsbio, tmpout);
		if (tmpin != dcont)
			BIO_free(tmpin);
		}
 	else
		{

		if (dcont && (tmpin == dcont))
			do_free_upto(cmsbio, dcont);
		else
			BIO_free_all(cmsbio);
		}

	if (tmpout && out != tmpout)
		BIO_free_all(tmpout);
 
 	if (cms_certs)
 		sk_X509_pop_free(cms_certs, X509_free);
	if (crls)
		sk_X509_CRL_pop_free(crls, X509_CRL_free);

	return ret;
	}
