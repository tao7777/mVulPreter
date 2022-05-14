usage(const char *prog)
{
	fprintf(stderr, "Usage: %s [OPTION...]\n", prog);
	fprintf(stderr, "  -f, --use-file=FILE          Use the specified configuration file\n");
#if defined _WITH_VRRP_ && defined _WITH_LVS_
	fprintf(stderr, "  -P, --vrrp                   Only run with VRRP subsystem\n");
	fprintf(stderr, "  -C, --check                  Only run with Health-checker subsystem\n");
#endif
#ifdef _WITH_BFD_
	fprintf(stderr, "  -B, --no_bfd                 Don't run BFD subsystem\n");
#endif
	fprintf(stderr, "      --all                    Force all child processes to run, even if have no configuration\n");
	fprintf(stderr, "  -l, --log-console            Log messages to local console\n");
	fprintf(stderr, "  -D, --log-detail             Detailed log messages\n");
	fprintf(stderr, "  -S, --log-facility=[0-7]     Set syslog facility to LOG_LOCAL[0-7]\n");
 	fprintf(stderr, "  -g, --log-file=FILE          Also log to FILE (default /tmp/keepalived.log)\n");
 	fprintf(stderr, "      --flush-log-file         Flush log file on write\n");
 	fprintf(stderr, "  -G, --no-syslog              Don't log via syslog\n");
 #ifdef _WITH_VRRP_
 	fprintf(stderr, "  -X, --release-vips           Drop VIP on transition from signal.\n");
 	fprintf(stderr, "  -V, --dont-release-vrrp      Don't remove VRRP VIPs and VROUTEs on daemon stop\n");
#endif
#ifdef _WITH_LVS_
	fprintf(stderr, "  -I, --dont-release-ipvs      Don't remove IPVS topology on daemon stop\n");
#endif
	fprintf(stderr, "  -R, --dont-respawn           Don't respawn child processes\n");
	fprintf(stderr, "  -n, --dont-fork              Don't fork the daemon process\n");
	fprintf(stderr, "  -d, --dump-conf              Dump the configuration data\n");
	fprintf(stderr, "  -p, --pid=FILE               Use specified pidfile for parent process\n");
#ifdef _WITH_VRRP_
	fprintf(stderr, "  -r, --vrrp_pid=FILE          Use specified pidfile for VRRP child process\n");
#endif
#ifdef _WITH_LVS_
	fprintf(stderr, "  -c, --checkers_pid=FILE      Use specified pidfile for checkers child process\n");
	fprintf(stderr, "  -a, --address-monitoring     Report all address additions/deletions notified via netlink\n");
#endif
#ifdef _WITH_BFD_
	fprintf(stderr, "  -b, --bfd_pid=FILE           Use specified pidfile for BFD child process\n");
#endif
#ifdef _WITH_SNMP_
	fprintf(stderr, "  -x, --snmp                   Enable SNMP subsystem\n");
	fprintf(stderr, "  -A, --snmp-agent-socket=FILE Use the specified socket for master agent\n");
#endif
#if HAVE_DECL_CLONE_NEWNET
	fprintf(stderr, "  -s, --namespace=NAME         Run in network namespace NAME (overrides config)\n");
#endif
	fprintf(stderr, "  -m, --core-dump              Produce core dump if terminate abnormally\n");
	fprintf(stderr, "  -M, --core-dump-pattern=PATN Also set /proc/sys/kernel/core_pattern to PATN (default 'core')\n");
#ifdef _MEM_CHECK_LOG_
	fprintf(stderr, "  -L, --mem-check-log          Log malloc/frees to syslog\n");
#endif
	fprintf(stderr, "  -i, --config-id id           Skip any configuration lines beginning '@' that don't match id\n"
			"                                or any lines beginning @^ that do match.\n"
			"                                The config-id defaults to the node name if option not used\n");
	fprintf(stderr, "      --signum=SIGFUNC         Return signal number for STOP, RELOAD, DATA, STATS"
#ifdef _WITH_JSON_
								", JSON"
#endif
								"\n");
	fprintf(stderr, "  -t, --config-test[=LOG_FILE] Check the configuration for obvious errors, output to\n"
			"                                stderr by default\n");
#ifdef _WITH_PERF_
	fprintf(stderr, "      --perf[=PERF_TYPE]       Collect perf data, PERF_TYPE=all, run(default) or end\n");
#endif
#ifdef WITH_DEBUG_OPTIONS
	fprintf(stderr, "      --debug[=...]            Enable debug options. p, b, c, v specify parent, bfd, checker and vrrp processes\n");
#ifdef _TIMER_CHECK_
	fprintf(stderr, "                                   T - timer debug\n");
#endif
#ifdef _SMTP_ALERT_DEBUG_
	fprintf(stderr, "                                   M - email alert debug\n");
#endif
#ifdef _EPOLL_DEBUG_
	fprintf(stderr, "                                   E - epoll debug\n");
#endif
#ifdef _EPOLL_THREAD_DUMP_
	fprintf(stderr, "                                   D - epoll thread dump debug\n");
#endif
#ifdef _VRRP_FD_DEBUG
	fprintf(stderr, "                                   F - vrrp fd dump debug\n");
#endif
#ifdef _REGEX_DEBUG_
	fprintf(stderr, "                                   R - regex debug\n");
#endif
#ifdef _WITH_REGEX_TIMERS_
	fprintf(stderr, "                                   X - regex timers\n");
#endif
#ifdef _TSM_DEBUG_
	fprintf(stderr, "                                   S - TSM debug\n");
#endif
#ifdef _NETLINK_TIMERS_
	fprintf(stderr, "                                   N - netlink timer debug\n");
#endif
	fprintf(stderr, "                                 Example --debug=TpMEvcp\n");
#endif
	fprintf(stderr, "  -v, --version                Display the version number\n");
	fprintf(stderr, "  -h, --help                   Display this help message\n");
}
