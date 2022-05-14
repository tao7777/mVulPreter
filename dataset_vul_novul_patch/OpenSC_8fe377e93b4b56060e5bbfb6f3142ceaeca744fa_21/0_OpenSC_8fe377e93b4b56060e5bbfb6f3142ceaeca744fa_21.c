static int insert_pin(
	sc_pkcs15_card_t *p15card,
	const char       *path,
	unsigned char     id,
	unsigned char     auth_id,
	unsigned char     pin_reference,
	int               min_length,
	const char       *label,
	int               pin_flags
){
	sc_card_t *card=p15card->card;
	sc_context_t *ctx=p15card->card->ctx;
	sc_file_t *f;
	struct sc_pkcs15_auth_info pin_info;
	struct sc_pkcs15_object pin_obj;
	int r;

	memset(&pin_info, 0, sizeof(pin_info));
	pin_info.auth_id.len      = 1;
	pin_info.auth_id.value[0] = id;
	pin_info.auth_type = SC_PKCS15_PIN_AUTH_TYPE_PIN;
	pin_info.attrs.pin.reference        = pin_reference;
	pin_info.attrs.pin.flags            = pin_flags;
	pin_info.attrs.pin.type             = SC_PKCS15_PIN_TYPE_ASCII_NUMERIC;
	pin_info.attrs.pin.min_length       = min_length;
	pin_info.attrs.pin.stored_length    = 16;
	pin_info.attrs.pin.max_length       = 16;
	pin_info.attrs.pin.pad_char         = '\0';
	pin_info.logged_in = SC_PIN_STATE_UNKNOWN;
	sc_format_path(path, &pin_info.path);

	memset(&pin_obj, 0, sizeof(pin_obj));
	strlcpy(pin_obj.label, label, sizeof(pin_obj.label));
	pin_obj.flags            = SC_PKCS15_CO_FLAG_MODIFIABLE | SC_PKCS15_CO_FLAG_PRIVATE;
	pin_obj.auth_id.len      = auth_id ? 0 : 1;
	pin_obj.auth_id.value[0] = auth_id;

	if(card->type==SC_CARD_TYPE_TCOS_V3){
		unsigned char buf[256];
		int i, rec_no=0;
		if(pin_info.path.len>=2) pin_info.path.len-=2;
		sc_append_file_id(&pin_info.path, 0x5049);
		if(sc_select_file(card, &pin_info.path, NULL)!=SC_SUCCESS){
			sc_debug(ctx, SC_LOG_DEBUG_NORMAL,
				"Select(%s) failed\n",
				sc_print_path(&pin_info.path));
			return 1;
		}
		sc_debug(ctx, SC_LOG_DEBUG_NORMAL,
			"Searching for PIN-Ref %02X\n", pin_reference);
		while((r=sc_read_record(card, ++rec_no, buf, sizeof(buf), SC_RECORD_BY_REC_NR))>0){
			int found=0, fbz=-1;
			if(buf[0]!=0xA0) continue;
			for(i=2;i<buf[1]+2;i+=2+buf[i+1]){
				if(buf[i]==0x83 && buf[i+1]==1 && buf[i+2]==pin_reference) ++found;
				if(buf[i]==0x90) fbz=buf[i+1+buf[i+1]];
			}
			if(found) pin_info.tries_left=fbz;
			if(found) break;
		}
		if(r<=0){
			sc_debug(ctx, SC_LOG_DEBUG_NORMAL,"No EF_PWDD-Record found\n");
 			return 1;
 		}
 	} else {
		if(sc_select_file(card, &pin_info.path, &f)!=SC_SUCCESS
			   	|| !f->prop_attr || f->prop_attr_len < 4){
 			sc_debug(ctx, SC_LOG_DEBUG_NORMAL,"Select(%s) failed\n", path);
 			return 1;
 		}
		pin_info.tries_left=f->prop_attr[3];
		sc_file_free(f);
	}

	r=sc_pkcs15emu_add_pin_obj(p15card, &pin_obj, &pin_info);
	if(r!=SC_SUCCESS){
		sc_debug(ctx, SC_LOG_DEBUG_NORMAL, "sc_pkcs15emu_add_pin_obj(%s) failed\n", path);
		return 4;
	}
	sc_debug(ctx, SC_LOG_DEBUG_NORMAL, "%s: OK, FBZ=%d\n", path, pin_info.tries_left);
	return 0;
}
