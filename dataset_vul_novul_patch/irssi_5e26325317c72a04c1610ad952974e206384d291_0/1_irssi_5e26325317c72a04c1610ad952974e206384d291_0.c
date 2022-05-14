 static void nick_hash_remove(CHANNEL_REC *channel, NICK_REC *nick)
 {
	NICK_REC *list;
 
 	list = g_hash_table_lookup(channel->nicks, nick->nick);
 	if (list == NULL)
 		return;
 
	if (list == nick || list->next == NULL) {
		g_hash_table_remove(channel->nicks, nick->nick);
		if (list->next != NULL) {
			g_hash_table_insert(channel->nicks, nick->next->nick,
					    nick->next);
		}
 	} else {
 		while (list->next != nick)
 			list = list->next;
 		list->next = nick->next;
 	}
 }
