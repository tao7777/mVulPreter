int nntp_add_group(char *line, void *data)
 {
   struct NntpServer *nserv = data;
   struct NntpData *nntp_data = NULL;
  char group[LONG_STRING];
   char desc[HUGE_STRING] = "";
   char mod;
   anum_t first, last;
 
   if (!nserv || !line)
     return 0;
 
  if (sscanf(line, "%s " ANUM " " ANUM " %c %[^\n]", group, &last, &first, &mod, desc) < 4)
     return 0;
 
   nntp_data = nntp_data_find(nserv, group);
   nntp_data->deleted = false;
  nntp_data->first_message = first;
  nntp_data->last_message = last;
  nntp_data->allowed = (mod == 'y') || (mod == 'm');
  mutt_str_replace(&nntp_data->desc, desc);
  if (nntp_data->newsrc_ent || nntp_data->last_cached)
    nntp_group_unread_stat(nntp_data);
  else if (nntp_data->last_message && nntp_data->first_message <= nntp_data->last_message)
    nntp_data->unread = nntp_data->last_message - nntp_data->first_message + 1;
  else
    nntp_data->unread = 0;
  return 0;
}
