static int muscle_list_files(sc_card_t *card, u8 *buf, size_t bufLen)
{
	muscle_private_t* priv = MUSCLE_DATA(card);
	mscfs_t *fs = priv->fs;
	int x;
	int count = 0;

 	mscfs_check_cache(priv->fs);
 
 	for(x = 0; x < fs->cache.size; x++) {
		u8* oid= fs->cache.array[x].objectId.id;
 		sc_debug(card->ctx, SC_LOG_DEBUG_NORMAL,
 			"FILE: %02X%02X%02X%02X\n",
 			oid[0],oid[1],oid[2],oid[3]);
		if(0 == memcmp(fs->currentPath, oid, 2)) {
			buf[0] = oid[2];
 			buf[1] = oid[3];
 			if(buf[0] == 0x00 && buf[1] == 0x00) continue; /* No directories/null names outside of root */
 			buf += 2;
			count+=2;
 		}
 	}
 	return count;
}
