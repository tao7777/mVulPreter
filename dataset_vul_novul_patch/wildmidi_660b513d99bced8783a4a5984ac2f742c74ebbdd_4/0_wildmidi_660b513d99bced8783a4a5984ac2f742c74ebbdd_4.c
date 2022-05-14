uint32_t _WM_SetupMidiEvent(struct _mdi *mdi, uint8_t * event_data, uint8_t running_event) {
uint32_t _WM_SetupMidiEvent(struct _mdi *mdi, uint8_t * event_data, uint32_t siz, uint8_t running_event) {
     /*
      Only add standard MIDI and Sysex events in here.
      Non-standard events need to be handled by calling function
     to avoid compatibility issues.

     TODO:
     Add value limit checks
     */
    uint32_t ret_cnt = 0;
    uint8_t command = 0;
    uint8_t channel = 0;
    uint8_t data_1 = 0;
     uint8_t data_2 = 0;
     char *text = NULL;
 
    if (!siz) goto shortbuf;

     if (event_data[0] >= 0x80) {
         command = *event_data & 0xf0;
         channel = *event_data++ & 0x0f;
         ret_cnt++;
        if (--siz == 0) goto shortbuf;
     } else {
         command = running_event & 0xf0;
         channel = running_event & 0x0f;
    }

     switch(command) {
         case 0x80:
         _SETUP_NOTEOFF:
            if (siz < 2) goto shortbuf;
             data_1 = *event_data++;
             data_2 = *event_data++;
             _WM_midi_setup_noteoff(mdi, channel, data_1, data_2);
             ret_cnt += 2;
             break;
         case 0x90:
             if (event_data[1] == 0) goto _SETUP_NOTEOFF; /* A velocity of 0 in a note on is actually a note off */
            if (siz < 2) goto shortbuf;
             data_1 = *event_data++;
             data_2 = *event_data++;
             midi_setup_noteon(mdi, channel, data_1, data_2);
             ret_cnt += 2;
             break;
         case 0xa0:
            if (siz < 2) goto shortbuf;
             data_1 = *event_data++;
             data_2 = *event_data++;
             midi_setup_aftertouch(mdi, channel, data_1, data_2);
             ret_cnt += 2;
             break;
         case 0xb0:
            if (siz < 2) goto shortbuf;
             data_1 = *event_data++;
             data_2 = *event_data++;
             midi_setup_control(mdi, channel, data_1, data_2);
            ret_cnt += 2;
            break;
        case 0xc0:
            data_1 = *event_data++;
            midi_setup_patch(mdi, channel, data_1);
            ret_cnt++;
            break;
        case 0xd0:
            data_1 = *event_data++;
            midi_setup_channel_pressure(mdi, channel, data_1);
             ret_cnt++;
             break;
         case 0xe0:
            if (siz < 2) goto shortbuf;
             data_1 = *event_data++;
             data_2 = *event_data++;
             midi_setup_pitch(mdi, channel, ((data_2 << 7) | (data_1 & 0x7f)));
            ret_cnt += 2;
            break;
        case 0xf0:
            if (channel == 0x0f) {
                /*
                 MIDI Meta Events
                 */
                uint32_t tmp_length = 0;
                if ((event_data[0] == 0x00) && (event_data[1] == 0x02)) {
                    /*
                      Sequence Number
                      We only setting this up here for WM_Event2Midi function
                      */
                    if (siz < 4) goto shortbuf;
                     midi_setup_sequenceno(mdi, ((event_data[2] << 8) + event_data[3]));
                     ret_cnt += 4;
                 } else if (event_data[0] == 0x01) {
                     /* Text Event */
                     /* Get Length */
                     event_data++;
                     ret_cnt++;
                    if (--siz && *event_data > 0x7f) {
                         do {
                            if (!siz) break;
                             tmp_length = (tmp_length << 7) + (*event_data & 0x7f);
                             event_data++;
                            siz--;
                             ret_cnt++;
                         } while (*event_data > 0x7f);
                     }
                    if (!siz) goto shortbuf;
                     tmp_length = (tmp_length << 7) + (*event_data & 0x7f);
                     event_data++;
                     ret_cnt++;
                    if (--siz < tmp_length) goto shortbuf;
                    if (!tmp_length) break; /* bad file? */
 
                     text = malloc(tmp_length + 1);
                     memcpy(text, event_data, tmp_length);
                    text[tmp_length] = '\0';
                    midi_setup_text(mdi, text);

                    ret_cnt += tmp_length;

                } else if (event_data[0] == 0x02) {
                    /* Copyright Event */
                     /* Get Length */
                     event_data++;
                     ret_cnt++;
                    if (--siz && *event_data > 0x7f) {
                         do {
                            if (!siz) break;
                             tmp_length = (tmp_length << 7) + (*event_data & 0x7f);
                             event_data++;
                            siz--;
                             ret_cnt++;
                         } while (*event_data > 0x7f);
                     }
                    if (!siz) goto shortbuf;
                     tmp_length = (tmp_length << 7) + (*event_data & 0x7f);
                     event_data++;
                     ret_cnt++;
                    if (--siz < tmp_length) goto shortbuf;
                    if (!tmp_length) break; /* bad file? */
 
                     /* Copy copyright info in the getinfo struct */
                     if (mdi->extra_info.copyright) {
                        mdi->extra_info.copyright = realloc(mdi->extra_info.copyright,(strlen(mdi->extra_info.copyright) + 1 + tmp_length + 1));
                        memcpy(&mdi->extra_info.copyright[strlen(mdi->extra_info.copyright) + 1], event_data, tmp_length);
                        mdi->extra_info.copyright[strlen(mdi->extra_info.copyright) + 1 + tmp_length] = '\0';
                        mdi->extra_info.copyright[strlen(mdi->extra_info.copyright)] = '\n';
                    } else {
                        mdi->extra_info.copyright = malloc(tmp_length + 1);
                        memcpy(mdi->extra_info.copyright, event_data, tmp_length);
                        mdi->extra_info.copyright[tmp_length] = '\0';
                    }

                    /* NOTE: free'd when events are cleared during closure of mdi */
                    text = malloc(tmp_length + 1);
                    memcpy(text, event_data, tmp_length);
                    text[tmp_length] = '\0';
                    midi_setup_copyright(mdi, text);

                    ret_cnt += tmp_length;

                } else if (event_data[0] == 0x03) {
                    /* Track Name Event */
                     /* Get Length */
                     event_data++;
                     ret_cnt++;
                    if (--siz && *event_data > 0x7f) {
                         do {
                            if (!siz) break;
                             tmp_length = (tmp_length << 7) + (*event_data & 0x7f);
                             event_data++;
                            siz--;
                             ret_cnt++;
                         } while (*event_data > 0x7f);
                     }
                    if (!siz) goto shortbuf;
                     tmp_length = (tmp_length << 7) + (*event_data & 0x7f);
                     event_data++;
                     ret_cnt++;
                    if (--siz < tmp_length) goto shortbuf;
                    if (!tmp_length) break; /* bad file? */
 
                     text = malloc(tmp_length + 1);
                     memcpy(text, event_data, tmp_length);
                    text[tmp_length] = '\0';
                    midi_setup_trackname(mdi, text);

                    ret_cnt += tmp_length;

                } else if (event_data[0] == 0x04) {
                    /* Instrument Name Event */
                     /* Get Length */
                     event_data++;
                     ret_cnt++;
                    if (--siz && *event_data > 0x7f) {
                         do {
                            if (!siz) break;
                             tmp_length = (tmp_length << 7) + (*event_data & 0x7f);
                             event_data++;
                            siz--;
                             ret_cnt++;
                         } while (*event_data > 0x7f);
                     }
                    if (!siz) goto shortbuf;
                     tmp_length = (tmp_length << 7) + (*event_data & 0x7f);
                     event_data++;
                     ret_cnt++;
                    if (--siz < tmp_length) goto shortbuf;
                    if (!tmp_length) break; /* bad file? */
 
                     text = malloc(tmp_length + 1);
                     memcpy(text, event_data, tmp_length);
                    text[tmp_length] = '\0';
                    midi_setup_instrumentname(mdi, text);

                    ret_cnt += tmp_length;

                } else if (event_data[0] == 0x05) {
                    /* Lyric Event */
                     /* Get Length */
                     event_data++;
                     ret_cnt++;
                    if (--siz && *event_data > 0x7f) {
                         do {
                            if (!siz) break;
                             tmp_length = (tmp_length << 7) + (*event_data & 0x7f);
                             event_data++;
                            siz--;
                             ret_cnt++;
                         } while (*event_data > 0x7f);
                     }
                    if (!siz) goto shortbuf;
                     tmp_length = (tmp_length << 7) + (*event_data & 0x7f);
                     event_data++;
                     ret_cnt++;
                    if (--siz < tmp_length) goto shortbuf;
                    if (!tmp_length) break; /* bad file? */
 
                     text = malloc(tmp_length + 1);
                     memcpy(text, event_data, tmp_length);
                    text[tmp_length] = '\0';
                    midi_setup_lyric(mdi, text);

                    ret_cnt += tmp_length;

                } else if (event_data[0] == 0x06) {
                    /* Marker Event */
                     /* Get Length */
                     event_data++;
                     ret_cnt++;
                    if (--siz && *event_data > 0x7f) {
                         do {
                            if (!siz) break;
                             tmp_length = (tmp_length << 7) + (*event_data & 0x7f);
                             event_data++;
                            siz--;
                             ret_cnt++;
                         } while (*event_data > 0x7f);
                     }
                    if (!siz) goto shortbuf;
                     tmp_length = (tmp_length << 7) + (*event_data & 0x7f);
                     event_data++;
                     ret_cnt++;
                    if (--siz < tmp_length) goto shortbuf;
                    if (!tmp_length) break; /* bad file? */
 
                     text = malloc(tmp_length + 1);
                     memcpy(text, event_data, tmp_length);
                    text[tmp_length] = '\0';
                    midi_setup_marker(mdi, text);

                    ret_cnt += tmp_length;

                } else if (event_data[0] == 0x07) {
                    /* Cue Point Event */
                     /* Get Length */
                     event_data++;
                     ret_cnt++;
                    if (--siz && *event_data > 0x7f) {
                         do {
                            if (!siz) break;
                             tmp_length = (tmp_length << 7) + (*event_data & 0x7f);
                             event_data++;
                            siz--;
                             ret_cnt++;
                         } while (*event_data > 0x7f);
                     }
                    if (!siz) goto shortbuf;
                     tmp_length = (tmp_length << 7) + (*event_data & 0x7f);
                     event_data++;
                     ret_cnt++;
                    if (--siz < tmp_length) goto shortbuf;
                    if (!tmp_length) break; /* bad file? */
 
                     text = malloc(tmp_length + 1);
                     memcpy(text, event_data, tmp_length);
                    text[tmp_length] = '\0';
                    midi_setup_cuepoint(mdi, text);

                    ret_cnt += tmp_length;

                } else if ((event_data[0] == 0x20) && (event_data[1] == 0x01)) {
                    /*
                      Channel Prefix
                      We only setting this up here for WM_Event2Midi function
                      */
                    if (siz < 3) goto shortbuf;
                     midi_setup_channelprefix(mdi, event_data[2]);
                     ret_cnt += 3;
                 } else if ((event_data[0] == 0x21) && (event_data[1] == 0x01)) {
                     /*
                      Port Prefix
                      We only setting this up here for WM_Event2Midi function
                      */
                    if (siz < 3) goto shortbuf;
                     midi_setup_portprefix(mdi, event_data[2]);
                     ret_cnt += 3;
                 } else if ((event_data[0] == 0x2F) && (event_data[1] == 0x00)) {
                    /*
                     End of Track
                      Deal with this inside calling function
                      We only setting this up here for _WM_Event2Midi function
                      */
                    if (siz < 2) goto shortbuf;
                     _WM_midi_setup_endoftrack(mdi);
                     ret_cnt += 2;
                 } else if ((event_data[0] == 0x51) && (event_data[1] == 0x03)) {
                    /*
                     Tempo
                      Deal with this inside calling function.
                      We only setting this up here for _WM_Event2Midi function
                      */
                    if (siz < 5) goto shortbuf;
                     _WM_midi_setup_tempo(mdi, ((event_data[2] << 16) + (event_data[3] << 8) + event_data[4]));
                     ret_cnt += 5;
                 } else if ((event_data[0] == 0x54) && (event_data[1] == 0x05)) {
                    if (siz < 7) goto shortbuf;
                     /*
                      SMPTE Offset
                      We only setting this up here for WM_Event2Midi function
                     */
                    midi_setup_smpteoffset(mdi, ((event_data[3] << 24) + (event_data[4] << 16) + (event_data[5] << 8) + event_data[6]));

                    /*
                     Because this has 5 bytes of data we gonna "hack" it a little
                     */
                    mdi->events[mdi->events_size - 1].event_data.channel = event_data[2];

                    ret_cnt += 7;
                } else if ((event_data[0] == 0x58) && (event_data[1] == 0x04)) {
                    /*
                      Time Signature
                      We only setting this up here for WM_Event2Midi function
                      */
                    if (siz < 6) goto shortbuf;
                     midi_setup_timesignature(mdi, ((event_data[2] << 24) + (event_data[3] << 16) + (event_data[4] << 8) + event_data[5]));
                     ret_cnt += 6;
                 } else if ((event_data[0] == 0x59) && (event_data[1] == 0x02)) {
                     /*
                      Key Signature
                      We only setting this up here for WM_Event2Midi function
                      */
                    if (siz < 4) goto shortbuf;
                     midi_setup_keysignature(mdi, ((event_data[2] << 8) + event_data[3]));
                     ret_cnt += 4;
                 } else {
                    /*
                     Unsupported Meta Event
                      */
                     event_data++;
                     ret_cnt++;
                    if (--siz && *event_data > 0x7f) {
                         do {
                            if (!siz) break;
                             tmp_length = (tmp_length << 7) + (*event_data & 0x7f);
                             event_data++;
                            siz--;
                             ret_cnt++;
                         } while (*event_data > 0x7f);
                     }
                    if (!siz) goto shortbuf;
                     tmp_length = (tmp_length << 7) + (*event_data & 0x7f);
                     ret_cnt++;
                     ret_cnt += tmp_length;
                    if (--siz < tmp_length) goto shortbuf;
                 }
 
             } else if ((channel == 0) || (channel == 7)) {
                /*
                 Sysex Events
                 */
                uint32_t sysex_len = 0;
                uint8_t *sysex_store = NULL;
 
                 if (*event_data > 0x7f) {
                     do {
                        if (!siz) break;
                         sysex_len = (sysex_len << 7) + (*event_data & 0x7F);
                         event_data++;
                        siz--;
                         ret_cnt++;
                     } while (*event_data > 0x7f);
                 }
                if (!siz) goto shortbuf;
                 sysex_len = (sysex_len << 7) + (*event_data & 0x7F);
                 event_data++;
                 ret_cnt++;
                if (--siz < sysex_len) goto shortbuf;
                if (!sysex_len) break; /* bad file? */
 
                 sysex_store = malloc(sizeof(uint8_t) * sysex_len);
                 memcpy(sysex_store, event_data, sysex_len);

                if (sysex_store[sysex_len - 1] == 0xF7) {
                    uint8_t rolandsysexid[] = { 0x41, 0x10, 0x42, 0x12 };
                    if (memcmp(rolandsysexid, sysex_store, 4) == 0) {
                        /* For Roland Sysex Messages */
                        /* checksum */
                        uint8_t sysex_cs = 0;
                        uint32_t sysex_ofs = 4;
                        do {
                            sysex_cs += sysex_store[sysex_ofs];
                            if (sysex_cs > 0x7F) {
                                sysex_cs -= 0x80;
                            }
                            sysex_ofs++;
                        } while (sysex_store[sysex_ofs + 1] != 0xf7);
                        sysex_cs = 128 - sysex_cs;
                        /* is roland sysex message valid */
                        if (sysex_cs == sysex_store[sysex_ofs]) {
                            /* process roland sysex event */
                            if (sysex_store[4] == 0x40) {
                                if (((sysex_store[5] & 0xf0) == 0x10) && (sysex_store[6] == 0x15)) {
                                    /* Roland Drum Track Setting */
                                    uint8_t sysex_ch = 0x0f & sysex_store[5];
                                    if (sysex_ch == 0x00) {
                                        sysex_ch = 0x09;
                                    } else if (sysex_ch <= 0x09) {
                                        sysex_ch -= 1;
                                    }
                                    midi_setup_sysex_roland_drum_track(mdi, sysex_ch, sysex_store[7]);
                                } else if ((sysex_store[5] == 0x00) && (sysex_store[6] == 0x7F) && (sysex_store[7] == 0x00)) {
                                    /* Roland GS Reset */
                                    midi_setup_sysex_roland_reset(mdi);
                                }
                            }
                        }
                    } else {
                        /* For non-Roland Sysex Messages */
                        uint8_t gm_reset[] = {0x7e, 0x7f, 0x09, 0x01, 0xf7};
                        uint8_t yamaha_reset[] = {0x43, 0x10, 0x4c, 0x00, 0x00, 0x7e, 0x00, 0xf7};

                        if (memcmp(gm_reset, sysex_store, 5) == 0) {
                            /* GM Reset */
                            midi_setup_sysex_gm_reset(mdi);
                        } else if (memcmp(yamaha_reset,sysex_store,8) == 0) {
                            /* Yamaha Reset */
                            midi_setup_sysex_yamaha_reset(mdi);
                        }
                    }
                }
                free(sysex_store);
                sysex_store = NULL;
                /*
                event_data += sysex_len;
                */
                ret_cnt += sysex_len;
            } else {
                _WM_GLOBAL_ERROR(__FUNCTION__, __LINE__, WM_ERR_CORUPT, "(unrecognized meta type event)", 0);
                return 0;
            }
            break;

        default: /* Should NEVER get here */
            ret_cnt = 0;
            break;
    }
     if (ret_cnt == 0)
         _WM_GLOBAL_ERROR(__FUNCTION__, __LINE__, WM_ERR_CORUPT, "(missing event)", 0);
     return ret_cnt;

shortbuf:
    _WM_GLOBAL_ERROR(__FUNCTION__, __LINE__, WM_ERR_CORUPT, "(too short)", 0);
    return 0;
 }
