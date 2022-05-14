WM_SYMBOL midi *WildMidi_OpenBuffer(uint8_t *midibuffer, uint32_t size) {
    uint8_t mus_hdr[] = { 'M', 'U', 'S', 0x1A };
    uint8_t xmi_hdr[] = { 'F', 'O', 'R', 'M' };
    midi * ret = NULL;

    if (!WM_Initialized) {
        _WM_GLOBAL_ERROR(__FUNCTION__, __LINE__, WM_ERR_NOT_INIT, NULL, 0);
        return (NULL);
    }
    if (midibuffer == NULL) {
        _WM_GLOBAL_ERROR(__FUNCTION__, __LINE__, WM_ERR_INVALID_ARG, "(NULL midi data buffer)", 0);
        return (NULL);
    }
    if (size > WM_MAXFILESIZE) {
        /* don't bother loading suspiciously long files */
         _WM_GLOBAL_ERROR(__FUNCTION__, __LINE__, WM_ERR_LONGFIL, NULL, 0);
         return (NULL);
     }
    if (size < 18) {
        _WM_GLOBAL_ERROR(__FUNCTION__, __LINE__, WM_ERR_CORUPT, "(too short)", 0);
        return (NULL);
    }
     if (memcmp(midibuffer,"HMIMIDIP", 8) == 0) {
         ret = (void *) _WM_ParseNewHmp(midibuffer, size);
     } else if (memcmp(midibuffer, "HMI-MIDISONG061595", 18) == 0) {
        ret = (void *) _WM_ParseNewHmi(midibuffer, size);
    } else if (memcmp(midibuffer, mus_hdr, 4) == 0) {
        ret = (void *) _WM_ParseNewMus(midibuffer, size);
    } else if (memcmp(midibuffer, xmi_hdr, 4) == 0) {
        ret = (void *) _WM_ParseNewXmi(midibuffer, size);
    } else {
        ret = (void *) _WM_ParseNewMidi(midibuffer, size);
    }

    if (ret) {
        if (add_handle(ret) != 0) {
            WildMidi_Close(ret);
            ret = NULL;
        }
    }

    return (ret);
}
