newkeys_from_blob(struct sshbuf *m, struct ssh *ssh, int mode)
{
	struct sshbuf *b = NULL;
	struct sshcomp *comp;
	struct sshenc *enc;
	struct sshmac *mac;
	struct newkeys *newkey = NULL;
	size_t keylen, ivlen, maclen;
	int r;

	if ((newkey = calloc(1, sizeof(*newkey))) == NULL) {
		r = SSH_ERR_ALLOC_FAIL;
		goto out;
	}
	if ((r = sshbuf_froms(m, &b)) != 0)
		goto out;
#ifdef DEBUG_PK
	sshbuf_dump(b, stderr);
#endif
	enc = &newkey->enc;
	mac = &newkey->mac;
	comp = &newkey->comp;

	if ((r = sshbuf_get_cstring(b, &enc->name, NULL)) != 0 ||
	    (r = sshbuf_get(b, &enc->cipher, sizeof(enc->cipher))) != 0 ||
	    (r = sshbuf_get_u32(b, (u_int *)&enc->enabled)) != 0 ||
	    (r = sshbuf_get_u32(b, &enc->block_size)) != 0 ||
	    (r = sshbuf_get_string(b, &enc->key, &keylen)) != 0 ||
	    (r = sshbuf_get_string(b, &enc->iv, &ivlen)) != 0)
		goto out;
	if (cipher_authlen(enc->cipher) == 0) {
		if ((r = sshbuf_get_cstring(b, &mac->name, NULL)) != 0)
			goto out;
		if ((r = mac_setup(mac, mac->name)) != 0)
			goto out;
		if ((r = sshbuf_get_u32(b, (u_int *)&mac->enabled)) != 0 ||
		    (r = sshbuf_get_string(b, &mac->key, &maclen)) != 0)
			goto out;
		if (maclen > mac->key_len) {
			r = SSH_ERR_INVALID_FORMAT;
			goto out;
		}
 		mac->key_len = maclen;
 	}
 	if ((r = sshbuf_get_u32(b, &comp->type)) != 0 ||
 	    (r = sshbuf_get_cstring(b, &comp->name, NULL)) != 0)
 		goto out;
 	if (enc->name == NULL ||
	    cipher_by_name(enc->name) != enc->cipher) {
		r = SSH_ERR_INVALID_FORMAT;
		goto out;
	}
	if (sshbuf_len(b) != 0) {
		r = SSH_ERR_INVALID_FORMAT;
		goto out;
	}
	enc->key_len = keylen;
	enc->iv_len = ivlen;
	ssh->kex->newkeys[mode] = newkey;
	newkey = NULL;
	r = 0;
 out:
	free(newkey);
	sshbuf_free(b);
	return r;
}
