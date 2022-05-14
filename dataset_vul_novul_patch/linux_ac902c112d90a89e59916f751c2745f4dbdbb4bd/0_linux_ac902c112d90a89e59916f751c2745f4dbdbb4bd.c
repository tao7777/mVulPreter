static bool snd_ctl_remove_numid_conflict(struct snd_card *card,
					  unsigned int count)
 {
 	struct snd_kcontrol *kctl;
 
	/* Make sure that the ids assigned to the control do not wrap around */
	if (card->last_numid >= UINT_MAX - count)
		card->last_numid = 0;

 	list_for_each_entry(kctl, &card->controls, list) {
 		if (kctl->id.numid < card->last_numid + 1 + count &&
 		    kctl->id.numid + kctl->count > card->last_numid + 1) {
		    	card->last_numid = kctl->id.numid + kctl->count - 1;
			return true;
		}
	}
	return false;
}
