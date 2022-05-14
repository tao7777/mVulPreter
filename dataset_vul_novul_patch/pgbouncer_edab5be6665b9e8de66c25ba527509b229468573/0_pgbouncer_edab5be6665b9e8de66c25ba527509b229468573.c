static bool check_client_passwd(PgSocket *client, const char *passwd)
{
	char md5[MD5_PASSWD_LEN + 1];
 	const char *correct;
 	PgUser *user = client->auth_user;
 
	/* auth_user may be missing */
	if (!user) {
		slog_error(client, "Password packet before auth packet?");
		return false;
	}

 	/* disallow empty passwords */
 	if (!*passwd || !*user->passwd)
 		return false;

	switch (cf_auth_type) {
	case AUTH_PLAIN:
		return strcmp(user->passwd, passwd) == 0;
	case AUTH_CRYPT:
		correct = crypt(user->passwd, (char *)client->tmp_login_salt);
		return correct && strcmp(correct, passwd) == 0;
	case AUTH_MD5:
		if (strlen(passwd) != MD5_PASSWD_LEN)
			return false;
		if (!isMD5(user->passwd))
			pg_md5_encrypt(user->passwd, user->name, strlen(user->name), user->passwd);
		pg_md5_encrypt(user->passwd + 3, (char *)client->tmp_login_salt, 4, md5);
		return strcmp(md5, passwd) == 0;
	}
	return false;
}
