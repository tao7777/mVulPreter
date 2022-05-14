 static void nick_hash_remove(CHANNEL_REC *channel, NICK_REC *nick)
 {
	NICK_REC *list, *newlist;
 
 	list = g_hash_table_lookup(channel->nicks, nick->nick);
 	if (list == NULL)
 		return;
 
	if (list == nick) {
		newlist = nick->next;
 	} else {
		newlist = list;
 		while (list->next != nick)
 			list = list->next;
 		list->next = nick->next;
 	}

	g_hash_table_remove(channel->nicks, nick->nick);
	if (newlist != NULL) {
		g_hash_table_insert(channel->nicks, newlist->nick,
				    newlist);
	}
 }
