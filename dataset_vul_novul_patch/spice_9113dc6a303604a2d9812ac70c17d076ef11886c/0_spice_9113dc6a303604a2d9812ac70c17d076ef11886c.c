vcard_apdu_new(unsigned char *raw_apdu, int len, vcard_7816_status_t *status)
{
    VCardAPDU *new_apdu;

    *status = VCARD7816_STATUS_EXC_ERROR_MEMORY_FAILURE;
    if (len < 4) {
        *status = VCARD7816_STATUS_ERROR_WRONG_LENGTH;
        return NULL;
    }

    new_apdu = g_new(VCardAPDU, 1);
    new_apdu->a_data = g_memdup(raw_apdu, len);
     new_apdu->a_len = len;
     *status = vcard_apdu_set_class(new_apdu);
     if (*status != VCARD7816_STATUS_SUCCESS) {
        vcard_apdu_delete(new_apdu);
         return NULL;
     }
     *status = vcard_apdu_set_length(new_apdu);
     if (*status != VCARD7816_STATUS_SUCCESS) {
        vcard_apdu_delete(new_apdu);
         new_apdu = NULL;
     }
     return new_apdu;
}
