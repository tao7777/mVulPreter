 vrrp_print_stats(void)
 {
	FILE *file = fopen_safe(stats_file, "w");
	element e;
	vrrp_t *vrrp;
 
 	if (!file) {
 		log_message(LOG_INFO, "Can't open %s (%d: %s)",
 			stats_file, errno, strerror(errno));
 		return;
 	}
 
 
	LIST_FOREACH(vrrp_data->vrrp, vrrp, e) {
 		fprintf(file, "VRRP Instance: %s\n", vrrp->iname);
 		fprintf(file, "  Advertisements:\n");
 		fprintf(file, "    Received: %" PRIu64 "\n", vrrp->stats->advert_rcvd);
 		fprintf(file, "    Sent: %d\n", vrrp->stats->advert_sent);
 		fprintf(file, "  Became master: %d\n", vrrp->stats->become_master);
		fprintf(file, "  Released master: %d\n", vrrp->stats->release_master);
 		fprintf(file, "  Packet Errors:\n");
 		fprintf(file, "    Length: %" PRIu64 "\n", vrrp->stats->packet_len_err);
 		fprintf(file, "    TTL: %" PRIu64 "\n", vrrp->stats->ip_ttl_err);
		fprintf(file, "    Invalid Type: %" PRIu64 "\n",
			vrrp->stats->invalid_type_rcvd);
		fprintf(file, "    Advertisement Interval: %" PRIu64 "\n",
			vrrp->stats->advert_interval_err);
		fprintf(file, "    Address List: %" PRIu64 "\n",
			vrrp->stats->addr_list_err);
		fprintf(file, "  Authentication Errors:\n");
		fprintf(file, "    Invalid Type: %d\n",
			vrrp->stats->invalid_authtype);
#ifdef _WITH_VRRP_AUTH_
		fprintf(file, "    Type Mismatch: %d\n",
			vrrp->stats->authtype_mismatch);
		fprintf(file, "    Failure: %d\n",
			vrrp->stats->auth_failure);
#endif
		fprintf(file, "  Priority Zero:\n");
		fprintf(file, "    Received: %" PRIu64 "\n", vrrp->stats->pri_zero_rcvd);
		fprintf(file, "    Sent: %" PRIu64 "\n", vrrp->stats->pri_zero_sent);
	}
	fclose(file);
}
