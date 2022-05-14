int tmx_check_pretran(sip_msg_t *msg)
{
	unsigned int chid;
	unsigned int slotid;
	int dsize;
	struct via_param *vbr;
	str scallid;
	str scseqmet;
	str scseqnum;
	str sftag;
	str svbranch = {NULL, 0};
	pretran_t *it;

	if(_tmx_ptran_table==NULL) {
		LM_ERR("pretran hash table not initialized yet\n");
		return -1;
	}
	if(get_route_type()!=REQUEST_ROUTE) {
		LM_ERR("invalid usage - not in request route\n");
		return -1;
	}
	if(msg->first_line.type!=SIP_REQUEST) {
		LM_ERR("invalid usage - not a sip request\n");
		return -1;
	}
	if(parse_headers(msg, HDR_FROM_F|HDR_VIA1_F|HDR_CALLID_F|HDR_CSEQ_F, 0)<0) {
		LM_ERR("failed to parse required headers\n");
		return -1;
	}
	if(msg->cseq==NULL || msg->cseq->parsed==NULL) {
		LM_ERR("failed to parse cseq headers\n");
		return -1;
	}
	if(get_cseq(msg)->method_id==METHOD_ACK
			|| get_cseq(msg)->method_id==METHOD_CANCEL) {
		LM_DBG("no pre-transaction management for ACK or CANCEL\n");
		return -1;
	}
	if (msg->via1==0) {
		LM_ERR("failed to get Via header\n");
		return -1;
	}
	if (parse_from_header(msg)<0 || get_from(msg)->tag_value.len==0) {
		LM_ERR("failed to get From header\n");
		return -1;
	}
	if (msg->callid==NULL || msg->callid->body.s==NULL) {
		LM_ERR("failed to parse callid headers\n");
		return -1;
	}

	vbr = msg->via1->branch;

	scallid = msg->callid->body;
	trim(&scallid);
	scseqmet = get_cseq(msg)->method;
	trim(&scseqmet);
	scseqnum = get_cseq(msg)->number;
	trim(&scseqnum);
	sftag = get_from(msg)->tag_value;
	trim(&sftag);

	chid = get_hash1_raw(msg->callid->body.s, msg->callid->body.len);
	slotid = chid & (_tmx_ptran_size-1);

	if(unlikely(_tmx_proc_ptran == NULL)) {
		_tmx_proc_ptran = (pretran_t*)shm_malloc(sizeof(pretran_t));
		if(_tmx_proc_ptran == NULL) {
			LM_ERR("not enough memory for pretran structure\n");
			return -1;
		}
		memset(_tmx_proc_ptran, 0, sizeof(pretran_t));
		_tmx_proc_ptran->pid = my_pid();
	}
	dsize = scallid.len + scseqnum.len + scseqmet.len
		+ sftag.len + 4;
 	if(likely(vbr!=NULL)) {
 		svbranch = vbr->value;
 		trim(&svbranch);
		dsize += svbranch.len + 1;
 	}
 	if(dsize<256) dsize = 256;
 
	tmx_pretran_unlink();

	if(dsize > _tmx_proc_ptran->dbuf.len) {
		if(_tmx_proc_ptran->dbuf.s) shm_free(_tmx_proc_ptran->dbuf.s);
		_tmx_proc_ptran->dbuf.s = (char*)shm_malloc(dsize);
		if(_tmx_proc_ptran->dbuf.s==NULL) {
			LM_ERR("not enough memory for pretran data\n");
			return -1;
		}
		_tmx_proc_ptran->dbuf.len = dsize;
	}
	_tmx_proc_ptran->hid = chid;
	_tmx_proc_ptran->cseqmetid = (get_cseq(msg))->method_id;

	_tmx_proc_ptran->callid.s = _tmx_proc_ptran->dbuf.s;
	memcpy(_tmx_proc_ptran->callid.s, scallid.s, scallid.len);
	_tmx_proc_ptran->callid.len = scallid.len;
	_tmx_proc_ptran->callid.s[_tmx_proc_ptran->callid.len] = '\0';

	_tmx_proc_ptran->ftag.s = _tmx_proc_ptran->callid.s
		+ _tmx_proc_ptran->callid.len + 1;
	memcpy(_tmx_proc_ptran->ftag.s, sftag.s, sftag.len);
	_tmx_proc_ptran->ftag.len = sftag.len;
	_tmx_proc_ptran->ftag.s[_tmx_proc_ptran->ftag.len] = '\0';

	_tmx_proc_ptran->cseqnum.s = _tmx_proc_ptran->ftag.s
		+ _tmx_proc_ptran->ftag.len + 1;
	memcpy(_tmx_proc_ptran->cseqnum.s, scseqnum.s, scseqnum.len);
	_tmx_proc_ptran->cseqnum.len = scseqnum.len;
	_tmx_proc_ptran->cseqnum.s[_tmx_proc_ptran->cseqnum.len] = '\0';

	_tmx_proc_ptran->cseqmet.s = _tmx_proc_ptran->cseqnum.s
		+ _tmx_proc_ptran->cseqnum.len + 1;
	memcpy(_tmx_proc_ptran->cseqmet.s, scseqmet.s, scseqmet.len);
	_tmx_proc_ptran->cseqmet.len = scseqmet.len;
	_tmx_proc_ptran->cseqmet.s[_tmx_proc_ptran->cseqmet.len] = '\0';

	if(likely(vbr!=NULL)) {
		_tmx_proc_ptran->vbranch.s = _tmx_proc_ptran->cseqmet.s
			+ _tmx_proc_ptran->cseqmet.len + 1;
		memcpy(_tmx_proc_ptran->vbranch.s, svbranch.s, svbranch.len);
		_tmx_proc_ptran->vbranch.len = svbranch.len;
		_tmx_proc_ptran->vbranch.s[_tmx_proc_ptran->vbranch.len] = '\0';
	} else {
		_tmx_proc_ptran->vbranch.s = NULL;
		_tmx_proc_ptran->vbranch.len = 0;
	}

	lock_get(&_tmx_ptran_table[slotid].lock);
	it = _tmx_ptran_table[slotid].plist;
	tmx_pretran_link_safe(slotid);
	for(; it!=NULL; it=it->next) {
		if(_tmx_proc_ptran->hid != it->hid
				|| _tmx_proc_ptran->cseqmetid != it->cseqmetid
				|| _tmx_proc_ptran->callid.len != it->callid.len
				|| _tmx_proc_ptran->ftag.len != it->ftag.len
				|| _tmx_proc_ptran->cseqmet.len != it->cseqmet.len
				|| _tmx_proc_ptran->cseqnum.len != it->cseqnum.len)
			continue;
		if(_tmx_proc_ptran->vbranch.s != NULL && it->vbranch.s != NULL) {
			if(_tmx_proc_ptran->vbranch.len != it->vbranch.len)
				continue;
			/* shortcut - check last char in Via branch
			 * - kamailio/ser adds there branch index => in case of paralel
			 *   forking by previous hop, catch it here quickly */
			if(_tmx_proc_ptran->vbranch.s[it->vbranch.len-1]
					!= it->vbranch.s[it->vbranch.len-1])
				continue;
			if(memcmp(_tmx_proc_ptran->vbranch.s,
						it->vbranch.s, it->vbranch.len)!=0)
				continue;
			/* shall stop by matching magic cookie?
			 *  if (vbr && vbr->value.s && vbr->value.len > MCOOKIE_LEN
			 *  && memcmp(vbr->value.s, MCOOKIE, MCOOKIE_LEN)==0) {
			 *  LM_DBG("rfc3261 cookie found in Via branch\n");
			 *  }
			 */
		}
		if(memcmp(_tmx_proc_ptran->callid.s,
					it->callid.s, it->callid.len)!=0
				|| memcmp(_tmx_proc_ptran->ftag.s,
					it->ftag.s, it->ftag.len)!=0
				|| memcmp(_tmx_proc_ptran->cseqnum.s,
					it->cseqnum.s, it->cseqnum.len)!=0)
			continue;
		if((it->cseqmetid==METHOD_OTHER || it->cseqmetid==METHOD_UNDEF)
				&& memcmp(_tmx_proc_ptran->cseqmet.s,
					it->cseqmet.s, it->cseqmet.len)!=0)
			continue;
		LM_DBG("matched another pre-transaction by pid %d for [%.*s]\n",
				it->pid, it->callid.len, it->callid.s);
		lock_release(&_tmx_ptran_table[slotid].lock);
		return 1;
	}
	lock_release(&_tmx_ptran_table[slotid].lock);
	return 0;
}
