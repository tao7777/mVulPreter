 construct_command_line(struct manager_ctx *manager, struct server *server)
 {
     static char cmd[BUF_SIZE];
     int i;
    int port;
 
    port = atoi(server->port);

    build_config(working_dir, manager, server);
 
     memset(cmd, 0, BUF_SIZE);
     snprintf(cmd, BUF_SIZE,
             "%s --manager-address %s -f %s/.shadowsocks_%d.pid -c %s/.shadowsocks_%d.conf",
             executable, manager->manager_address, working_dir, port, working_dir, port);
 
     if (manager->acl != NULL) {
         int len = strlen(cmd);
        snprintf(cmd + len, BUF_SIZE - len, " --acl %s", manager->acl);
    }
    if (manager->timeout != NULL) {
        int len = strlen(cmd);
        snprintf(cmd + len, BUF_SIZE - len, " -t %s", manager->timeout);
    }
#ifdef HAVE_SETRLIMIT
    if (manager->nofile) {
        int len = strlen(cmd);
        snprintf(cmd + len, BUF_SIZE - len, " -n %d", manager->nofile);
    }
#endif
    if (manager->user != NULL) {
        int len = strlen(cmd);
        snprintf(cmd + len, BUF_SIZE - len, " -a %s", manager->user);
    }
    if (manager->verbose) {
        int len = strlen(cmd);
        snprintf(cmd + len, BUF_SIZE - len, " -v");
    }
    if (server->mode == NULL && manager->mode == UDP_ONLY) {
        int len = strlen(cmd);
        snprintf(cmd + len, BUF_SIZE - len, " -U");
    }
    if (server->mode == NULL && manager->mode == TCP_AND_UDP) {
        int len = strlen(cmd);
        snprintf(cmd + len, BUF_SIZE - len, " -u");
    }
    if (server->fast_open[0] == 0 && manager->fast_open) {
        int len = strlen(cmd);
        snprintf(cmd + len, BUF_SIZE - len, " --fast-open");
    }
    if (manager->ipv6first) {
        int len = strlen(cmd);
        snprintf(cmd + len, BUF_SIZE - len, " -6");
    }
    if (manager->mtu) {
        int len = strlen(cmd);
        snprintf(cmd + len, BUF_SIZE - len, " --mtu %d", manager->mtu);
    }
    if (server->plugin == NULL && manager->plugin) {
        int len = strlen(cmd);
        snprintf(cmd + len, BUF_SIZE - len, " --plugin \"%s\"", manager->plugin);
    }
    if (server->plugin_opts == NULL && manager->plugin_opts) {
        int len = strlen(cmd);
        snprintf(cmd + len, BUF_SIZE - len, " --plugin-opts \"%s\"", manager->plugin_opts);
    }
    for (i = 0; i < manager->nameserver_num; i++) {
        int len = strlen(cmd);
        snprintf(cmd + len, BUF_SIZE - len, " -d %s", manager->nameservers[i]);
    }
    for (i = 0; i < manager->host_num; i++) {
        int len = strlen(cmd);
        snprintf(cmd + len, BUF_SIZE - len, " -s %s", manager->hosts[i]);
    }
    {
        int len = strlen(cmd);
        snprintf(cmd + len, BUF_SIZE - len, " --reuse-port");
    }

    if (verbose) {
        LOGI("cmd: %s", cmd);
    }

    return cmd;
}
