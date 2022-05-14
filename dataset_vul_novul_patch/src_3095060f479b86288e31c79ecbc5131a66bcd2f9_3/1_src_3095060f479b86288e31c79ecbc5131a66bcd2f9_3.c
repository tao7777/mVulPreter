newkeys_to_blob(struct sshbuf *m, struct ssh *ssh, int mode)
{
	struct sshbuf *b;
	struct sshcipher_ctx *cc;
	struct sshcomp *comp;
	struct sshenc *enc;
	struct sshmac *mac;
	struct newkeys *newkey;
	int r;

	if ((newkey = ssh->state->newkeys[mode]) == NULL)
		return SSH_ERR_INTERNAL_ERROR;
	enc = &newkey->enc;
	mac = &newkey->mac;
	comp = &newkey->comp;
	cc = (mode == MODE_OUT) ? ssh->state->send_context :
	    ssh->state->receive_context;
	if ((r = cipher_get_keyiv(cc, enc->iv, enc->iv_len)) != 0)
		return r;
	if ((b = sshbuf_new()) == NULL)
		return SSH_ERR_ALLOC_FAIL;
	/* The cipher struct is constant and shared, you export pointer */
	if ((r = sshbuf_put_cstring(b, enc->name)) != 0 ||
	    (r = sshbuf_put(b, &enc->cipher, sizeof(enc->cipher))) != 0 ||
	    (r = sshbuf_put_u32(b, enc->enabled)) != 0 ||
	    (r = sshbuf_put_u32(b, enc->block_size)) != 0 ||
	    (r = sshbuf_put_string(b, enc->key, enc->key_len)) != 0 ||
	    (r = sshbuf_put_string(b, enc->iv, enc->iv_len)) != 0)
		goto out;
	if (cipher_authlen(enc->cipher) == 0) {
		if ((r = sshbuf_put_cstring(b, mac->name)) != 0 ||
		    (r = sshbuf_put_u32(b, mac->enabled)) != 0 ||
		    (r = sshbuf_put_string(b, mac->key, mac->key_len)) != 0)
 			goto out;
 	}
 	if ((r = sshbuf_put_u32(b, comp->type)) != 0 ||
	    (r = sshbuf_put_u32(b, comp->enabled)) != 0 ||
 	    (r = sshbuf_put_cstring(b, comp->name)) != 0)
 		goto out;
 	r = sshbuf_put_stringb(m, b);
 out:
	sshbuf_free(b);
	return r;
}
