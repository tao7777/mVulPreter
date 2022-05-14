cib_connect(gboolean full)
{
    int rc = pcmk_ok;
    static gboolean need_pass = TRUE;

    CRM_CHECK(cib != NULL, return -EINVAL);

    if (getenv("CIB_passwd") != NULL) {
        need_pass = FALSE;
    }

    if(watch_fencing && st == NULL) {
        st = stonith_api_new();
    }
    
    if(watch_fencing && st->state == stonith_disconnected) {
        crm_trace("Connecting to stonith");
        rc = st->cmds->connect(st, crm_system_name, NULL);
        if(rc == pcmk_ok) {
            crm_trace("Setting up stonith callbacks");
            st->cmds->register_notification(st, T_STONITH_NOTIFY_FENCE, mon_st_callback);
        }
    }
    
    if (cib->state != cib_connected_query && cib->state != cib_connected_command) {
        crm_trace("Connecting to the CIB");
        if (as_console && need_pass && cib->variant == cib_remote) {
            need_pass = FALSE;
            print_as("Password:");
        }

        rc = cib->cmds->signon(cib, crm_system_name, cib_query);

        if (rc != pcmk_ok) {
            return rc;
        }

        current_cib = get_cib_copy(cib);
        mon_refresh_display(NULL);

        if (full) {
             if (rc == pcmk_ok) {
                 rc = cib->cmds->set_connection_dnotify(cib, mon_cib_connection_destroy);
                 if (rc == -EPROTONOSUPPORT) {
                    print_as("Notification setup not supported, won't be able to reconnect after failure");
                     if (as_console) {
                         sleep(2);
                     }
                    rc = pcmk_ok;
                }

            }

            if (rc == pcmk_ok) {
                cib->cmds->del_notify_callback(cib, T_CIB_DIFF_NOTIFY, crm_diff_update);
                rc = cib->cmds->add_notify_callback(cib, T_CIB_DIFF_NOTIFY, crm_diff_update);
            }

            if (rc != pcmk_ok) {
                print_as("Notification setup failed, could not monitor CIB actions");
                if (as_console) {
                    sleep(2);
                }
                clean_up(-rc);
            }
        }
    }
    return rc;
}
