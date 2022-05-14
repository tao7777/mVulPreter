daemon_AuthUserPwd(char *username, char *password, char *errbuf)
{
#ifdef _WIN32
	/*
	 * Warning: the user which launches the process must have the
	 * SE_TCB_NAME right.
	 * This corresponds to have the "Act as part of the Operating System"
	 * turned on (administrative tools, local security settings, local
	 * policies, user right assignment)
	 * However, it seems to me that if you run it as a service, this
	 * right should be provided by default.
	 *
	 * XXX - hopefully, this returns errors such as ERROR_LOGON_FAILURE,
	 * which merely indicates that the user name or password is
	 * incorrect, not whether it's the user name or the password
	 * that's incorrect, so a client that's trying to brute-force
	 * accounts doesn't know whether it's the user name or the
	 * password that's incorrect, so it doesn't know whether to
 	 * stop trying to log in with a given user name and move on
 	 * to another user name.
 	 */
	DWORD error;
 	HANDLE Token;
	char errmsgbuf[PCAP_ERRBUF_SIZE];	// buffer for errors to log

 	if (LogonUser(username, ".", password, LOGON32_LOGON_NETWORK, LOGON32_PROVIDER_DEFAULT, &Token) == 0)
 	{
		pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "Authentication failed");
		error = GetLastError();
		if (error != ERROR_LOGON_FAILURE)
		{
			// Some error other than an authentication error;
			// log it.
			pcap_fmt_errmsg_for_win32_err(errmsgbuf,
			    PCAP_ERRBUF_SIZE, error, "LogonUser() failed");
			rpcapd_log(LOGPRIO_ERROR, "%s", errmsgbuf);
		}			    
 		return -1;
 	}
 
 	if (ImpersonateLoggedOnUser(Token) == 0)
 	{
		pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "Authentication failed");
		pcap_fmt_errmsg_for_win32_err(errmsgbuf, PCAP_ERRBUF_SIZE,
 		    GetLastError(), "ImpersonateLoggedOnUser() failed");
		rpcapd_log(LOGPRIO_ERROR, "%s", errmsgbuf);
 		CloseHandle(Token);
 		return -1;
 	}

	CloseHandle(Token);
	return 0;

#else
	/*
	 * See
	 *
	 *	http://www.unixpapa.com/incnote/passwd.html
	 *
	 * We use the Solaris/Linux shadow password authentication if
	 * we have getspnam(), otherwise we just do traditional
	 * authentication, which, on some platforms, might work, even
	 * with shadow passwords, if we're running as root.  Traditional
	 * authenticaion won't work if we're not running as root, as
	 * I think these days all UN*Xes either won't return the password
	 * at all with getpwnam() or will only do so if you're root.
	 *
	 * XXX - perhaps what we *should* be using is PAM, if we have
	 * it.  That might hide all the details of username/password
	 * authentication, whether it's done with a visible-to-root-
 	 * only password database or some other authentication mechanism,
 	 * behind its API.
 	 */
	int error;
 	struct passwd *user;
 	char *user_password;
 #ifdef HAVE_GETSPNAM
	struct spwd *usersp;
#endif
	char *crypt_password;

 	if ((user = getpwnam(username)) == NULL)
 	{
		pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "Authentication failed");
 		return -1;
 	}
 
 #ifdef HAVE_GETSPNAM
 	if ((usersp = getspnam(username)) == NULL)
 	{
		pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "Authentication failed");
 		return -1;
 	}
 	user_password = usersp->sp_pwdp;
#else
	/*
	 * XXX - what about other platforms?
	 * The unixpapa.com page claims this Just Works on *BSD if you're
	 * running as root - it's from 2000, so it doesn't indicate whether
	 * macOS (which didn't come out until 2001, under the name Mac OS
	 * X) behaves like the *BSDs or not, and might also work on AIX.
	 * HP-UX does something else.
	 *
	 * Again, hopefully PAM hides all that.
	 */
 	user_password = user->pw_passwd;
 #endif
 
	//
	// The Single UNIX Specification says that if crypt() fails it
	// sets errno, but some implementatons that haven't been run
	// through the SUS test suite might not do so.
	//
	errno = 0;
 	crypt_password = crypt(password, user_password);
 	if (crypt_password == NULL)
 	{
		error = errno;
 		pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "Authentication failed");
		if (error == 0)
		{
			// It didn't set errno.
			rpcapd_log(LOGPRIO_ERROR, "crypt() failed");
		}
		else
		{
			rpcapd_log(LOGPRIO_ERROR, "crypt() failed: %s",
			    strerror(error));
		}
 		return -1;
 	}
 	if (strcmp(user_password, crypt_password) != 0)
 	{
		pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE, "Authentication failed");
 		return -1;
 	}
 
 	if (setuid(user->pw_uid))
 	{
		error = errno;
 		pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
		    error, "setuid");
		rpcapd_log(LOGPRIO_ERROR, "setuid() failed: %s",
		    strerror(error));
 		return -1;
 	}
 
 /*	if (setgid(user->pw_gid))
 	{
		error = errno;
 		pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
 		    errno, "setgid");
		rpcapd_log(LOGPRIO_ERROR, "setgid() failed: %s",
		    strerror(error));
 		return -1;
 	}
 */
	return 0;

#endif

}
