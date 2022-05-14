open_log_file(const char *name, const char *prog, const char *namespace, const char *instance)
{
	char *file_name;

	if (log_file) {
		fclose(log_file);
		log_file = NULL;
	}

	if (!name)
		return;
 
 	file_name = make_file_name(name, prog, namespace, instance);
 
	log_file = fopen_safe(file_name, "a");
 	if (log_file) {
 		int n = fileno(log_file);
 		fcntl(n, F_SETFD, FD_CLOEXEC | fcntl(n, F_GETFD));
		fcntl(n, F_SETFL, O_NONBLOCK | fcntl(n, F_GETFL));
	}

	FREE(file_name);
}
