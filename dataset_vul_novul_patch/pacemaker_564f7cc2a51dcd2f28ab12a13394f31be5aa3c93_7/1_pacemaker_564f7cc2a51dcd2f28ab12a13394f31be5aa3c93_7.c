cib_remote_signon(cib_t * cib, const char *name, enum cib_conn_type type)
{
    int rc = pcmk_ok;
    cib_remote_opaque_t *private = cib->variant_opaque;

    if (private->passwd == NULL) {
        struct termios settings;
        int rc;

        rc = tcgetattr(0, &settings);
        settings.c_lflag &= ~ECHO;
        rc = tcsetattr(0, TCSANOW, &settings);

        fprintf(stderr, "Password: ");
        private->passwd = calloc(1, 1024);
        rc = scanf("%s", private->passwd);
        fprintf(stdout, "\n");
        /* fprintf(stderr, "entered: '%s'\n", buffer); */
        if (rc < 1) {
            private->passwd = NULL;
        }

        settings.c_lflag |= ECHO;
        rc = tcsetattr(0, TCSANOW, &settings);
    }

    if (private->server == NULL || private->user == NULL) {
        rc = -EINVAL;
     }
 
     if (rc == pcmk_ok) {
        rc = cib_tls_signon(cib, &(private->command));
     }
 
     if (rc == pcmk_ok) {
        rc = cib_tls_signon(cib, &(private->callback));
     }
 
     if (rc == pcmk_ok) {
        xmlNode *hello =
            cib_create_op(0, private->callback.token, CRM_OP_REGISTER, NULL, NULL, NULL, 0, NULL);
        crm_xml_add(hello, F_CIB_CLIENTNAME, name);
        crm_send_remote_msg(private->command.session, hello, private->command.encrypted);
        free_xml(hello);
    }

    if (rc == pcmk_ok) {
        fprintf(stderr, "%s: Opened connection to %s:%d\n", name, private->server, private->port);
        cib->state = cib_connected_command;
        cib->type = cib_command;

    } else {
        fprintf(stderr, "%s: Connection to %s:%d failed: %s\n",
                name, private->server, private->port, pcmk_strerror(rc));
    }

    return rc;
}
