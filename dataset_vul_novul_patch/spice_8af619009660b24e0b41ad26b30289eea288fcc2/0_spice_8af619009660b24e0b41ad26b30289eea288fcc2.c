 static void reds_handle_ticket(void *opaque)
 {
     RedLinkInfo *link = (RedLinkInfo *)opaque;
    char *password;
     time_t ltime;
    int password_size;
 
     time(&ltime);
    if (RSA_size(link->tiTicketing.rsa) < SPICE_MAX_PASSWORD_LENGTH) {
        spice_warning("RSA modulus size is smaller than SPICE_MAX_PASSWORD_LENGTH (%d < %d), "
                      "SPICE ticket sent from client may be truncated",
                      RSA_size(link->tiTicketing.rsa), SPICE_MAX_PASSWORD_LENGTH);
    }

    password = g_malloc0(RSA_size(link->tiTicketing.rsa) + 1);
    password_size = RSA_private_decrypt(link->tiTicketing.rsa_size,
                                        link->tiTicketing.encrypted_ticket.encrypted_data,
                                        (unsigned char *)password,
                                        link->tiTicketing.rsa,
                                        RSA_PKCS1_OAEP_PADDING);
    if (password_size == -1) {
        spice_warning("failed to decrypt RSA encrypted password: %s",
                      ERR_error_string(ERR_get_error(), NULL));
        goto error;
    }
    password[password_size] = '\0';
 
     if (ticketing_enabled && !link->skip_auth) {
         int expired =  taTicket.expiration_time < ltime;
 
         if (strlen(taTicket.password) == 0) {
             spice_warning("Ticketing is enabled, but no password is set. "
                          "please set a ticket first");
            goto error;
         }
 
        if (expired || strcmp(password, taTicket.password) != 0) {
             if (expired) {
                 spice_warning("Ticket has expired");
             } else {
                 spice_warning("Invalid password");
             }
            goto error;
         }
     }
 
     reds_handle_link(link);
    goto end;

error:
    reds_send_link_result(link, SPICE_LINK_ERR_PERMISSION_DENIED);
    reds_link_free(link);

end:
    g_free(password);
 }
