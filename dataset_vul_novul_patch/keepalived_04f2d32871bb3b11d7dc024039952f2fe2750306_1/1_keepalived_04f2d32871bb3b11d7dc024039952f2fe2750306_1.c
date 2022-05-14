 smtp_log_to_file(smtp_t *smtp)
 {
	FILE *fp = fopen("/tmp/smtp-alert.log", "a");
 	time_t now;
 	struct tm tm;
 	char time_buf[25];
	int time_buf_len;

	time(&now);
	localtime_r(&now, &tm);
	time_buf_len = strftime(time_buf, sizeof time_buf, "%a %b %e %X %Y", &tm);

	fprintf(fp, "%s: %s -> %s\n"
		    "%*sSubject: %s\n"
		    "%*sBody:    %s\n\n",
		    time_buf, global_data->email_from, smtp->email_to,
		    time_buf_len - 7, "", smtp->subject,
		    time_buf_len - 7, "", smtp->body);

	fclose(fp);

	free_smtp_all(smtp);
}
