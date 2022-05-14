_WM_ParseNewMidi(uint8_t *midi_data, uint32_t midi_size) {
    struct _mdi *mdi;
 
     uint32_t tmp_val;
     uint32_t midi_type;
    uint32_t track_size;
     uint8_t **tracks;
     uint32_t end_of_tracks = 0;
     uint32_t no_tracks;
     uint32_t i;
    uint32_t divisions = 96;
    uint32_t tempo = 500000;
    float samples_per_delta_f = 0.0;

    uint32_t sample_count = 0;
     float sample_count_f = 0.0;
     float sample_remainder = 0.0;
     uint8_t *sysex_store = NULL;
     uint32_t *track_delta;
     uint8_t *track_end;
     uint32_t smallest_delta = 0;
     uint32_t subtract_delta = 0;
     uint32_t setup_ret = 0;
 
    if (midi_size < 14) {
        _WM_GLOBAL_ERROR(__FUNCTION__, __LINE__, WM_ERR_CORUPT, "(too short)", 0);
        return (NULL);
    }

    if (!memcmp(midi_data, "RIFF", 4)) {
        if (midi_size < 34) {
            _WM_GLOBAL_ERROR(__FUNCTION__, __LINE__, WM_ERR_CORUPT, "(too short)", 0);
            return (NULL);
        }
        midi_data += 20;
        midi_size -= 20;
    }

    if (memcmp(midi_data, "MThd", 4)) {
        _WM_GLOBAL_ERROR(__FUNCTION__, __LINE__, WM_ERR_NOT_MIDI, NULL, 0);
        return (NULL);
    }
    midi_data += 4;
    midi_size -= 4;

    /*
     * Get Midi Header Size - must always be 6
     */
    tmp_val = *midi_data++ << 24;
    tmp_val |= *midi_data++ << 16;
    tmp_val |= *midi_data++ << 8;
    tmp_val |= *midi_data++;
    midi_size -= 4;
    if (tmp_val != 6) {
        _WM_GLOBAL_ERROR(__FUNCTION__, __LINE__, WM_ERR_CORUPT, NULL, 0);
        return (NULL);
    }

    /*
     * Get Midi Format - we only support 0, 1 & 2
     */
    tmp_val = *midi_data++ << 8;
    tmp_val |= *midi_data++;
    midi_size -= 2;
    if (tmp_val > 2) {
        _WM_GLOBAL_ERROR(__FUNCTION__, __LINE__, WM_ERR_INVALID, NULL, 0);
        return (NULL);
    }
    midi_type = tmp_val;

    /*
     * Get No. of Tracks
     */
    tmp_val = *midi_data++ << 8;
    tmp_val |= *midi_data++;
    midi_size -= 2;
    if (tmp_val < 1) {
        _WM_GLOBAL_ERROR(__FUNCTION__, __LINE__, WM_ERR_CORUPT, "(no tracks)", 0);
        return (NULL);
    }
    no_tracks = tmp_val;

    /*
     * Check that type 0 midi file has only 1 track
     */
    if ((midi_type == 0) && (no_tracks > 1)) {
        _WM_GLOBAL_ERROR(__FUNCTION__, __LINE__, WM_ERR_INVALID, "(expected 1 track for type 0 midi file, found more)", 0);
        return (NULL);
    }

    /*
     * Get Divisions
     */
    divisions = *midi_data++ << 8;
    divisions |= *midi_data++;
    midi_size -= 2;
    if (divisions & 0x00008000) {
        _WM_GLOBAL_ERROR(__FUNCTION__, __LINE__, WM_ERR_INVALID, NULL, 0);
        return (NULL);
    }

    samples_per_delta_f = _WM_GetSamplesPerTick(divisions, tempo);

    mdi = _WM_initMDI();
     _WM_midi_setup_divisions(mdi,divisions);
 
     tracks = malloc(sizeof(uint8_t *) * no_tracks);
     track_delta = malloc(sizeof(uint32_t) * no_tracks);
     track_end = malloc(sizeof(uint8_t) * no_tracks);
     running_event = malloc(sizeof(uint8_t) * no_tracks);

    smallest_delta = 0xffffffff;
    for (i = 0; i < no_tracks; i++) {
        if (midi_size < 8) {
            _WM_GLOBAL_ERROR(__FUNCTION__, __LINE__, WM_ERR_CORUPT, "(too short)", 0);
            goto _end;
        }
        if (memcmp(midi_data, "MTrk", 4) != 0) {
            _WM_GLOBAL_ERROR(__FUNCTION__, __LINE__, WM_ERR_CORUPT, "(missing track header)", 0);
            goto _end;
        }
         midi_data += 4;
         midi_size -= 4;
 
        track_size = *midi_data++ << 24;
        track_size |= *midi_data++ << 16;
        track_size |= *midi_data++ << 8;
        track_size |= *midi_data++;
         midi_size -= 4;
        if (midi_size < track_size) {
             _WM_GLOBAL_ERROR(__FUNCTION__, __LINE__, WM_ERR_CORUPT, "(too short)", 0);
             goto _end;
         }
        if (track_size < 3) {
             _WM_GLOBAL_ERROR(__FUNCTION__, __LINE__, WM_ERR_CORUPT, "(bad track size)", 0);
             goto _end;
         }
        if ((midi_data[track_size - 3] != 0xFF)
                || (midi_data[track_size - 2] != 0x2F)
                || (midi_data[track_size - 1] != 0x00)) {
             _WM_GLOBAL_ERROR(__FUNCTION__, __LINE__, WM_ERR_CORUPT, "(missing EOT)", 0);
             goto _end;
         }
         tracks[i] = midi_data;
        midi_data += track_size;
        midi_size -= track_size;
         track_end[i] = 0;
         running_event[i] = 0;
         track_delta[i] = 0;
 
         while (*tracks[i] > 0x7F) {
             track_delta[i] = (track_delta[i] << 7) + (*tracks[i] & 0x7F);
             tracks[i]++;
         }
         track_delta[i] = (track_delta[i] << 7) + (*tracks[i] & 0x7F);
         tracks[i]++;
 
         if (midi_type == 1 ) {
             if (track_delta[i] < smallest_delta) {
                smallest_delta = track_delta[i];
            }
        } else {
            /*
             * Type 0 & 2 midi only needs delta from 1st track
             * for initial sample calculations.
             */
            if (i == 0) smallest_delta = track_delta[i];
        }
    }

    subtract_delta = smallest_delta;
    sample_count_f = (((float) smallest_delta * samples_per_delta_f) + sample_remainder);
    sample_count = (uint32_t) sample_count_f;
    sample_remainder = sample_count_f - (float) sample_count;

    mdi->events[mdi->event_count - 1].samples_to_next += sample_count;
    mdi->extra_info.approx_total_samples += sample_count;

    /*
     * Handle type 0 & 2 the same, but type 1 differently
     */
    if (midi_type == 1) {
        /* Type 1 */
        while (end_of_tracks != no_tracks) {
            smallest_delta = 0;
            for (i = 0; i < no_tracks; i++) {
                if (track_end[i])
                    continue;
                if (track_delta[i]) {
                    track_delta[i] -= subtract_delta;
                    if (track_delta[i]) {
                        if ((!smallest_delta)
                             || (smallest_delta > track_delta[i])) {
                            smallest_delta = track_delta[i];
                        }
                        continue;
                     }
                 }
                 do {
                    setup_ret = _WM_SetupMidiEvent(mdi, tracks[i], running_event[i]);
                     if (setup_ret == 0) {
                         goto _end;
                     }
                    if (tracks[i][0] > 0x7f) {
                        if (tracks[i][0] < 0xf0) {
                            /* Events 0x80 - 0xef set running event */
                            running_event[i] = tracks[i][0];
                        } else if ((tracks[i][0] == 0xf0) || (tracks[i][0] == 0xf7)) {
                            /* Sysex resets running event */
                            running_event[i] = 0;
                        } else if ((tracks[i][0] == 0xff) && (tracks[i][1] == 0x2f) && (tracks[i][2] == 0x00)) {
                            /* End of Track */
                             end_of_tracks++;
                             track_end[i] = 1;
                             tracks[i] += 3;
                             goto NEXT_TRACK;
                         } else if ((tracks[i][0] == 0xff) && (tracks[i][1] == 0x51) && (tracks[i][2] == 0x03)) {
                             /* Tempo */
                            tempo = (tracks[i][3] << 16) + (tracks[i][4] << 8)+ tracks[i][5];
                            if (!tempo)
                                tempo = 500000;

                            samples_per_delta_f = _WM_GetSamplesPerTick(divisions, tempo);
                         }
                     }
                     tracks[i] += setup_ret;
 
                     if (*tracks[i] > 0x7f) {
                         do {
                             track_delta[i] = (track_delta[i] << 7) + (*tracks[i] & 0x7F);
                             tracks[i]++;
                         } while (*tracks[i] > 0x7f);
                     }
                     track_delta[i] = (track_delta[i] << 7) + (*tracks[i] & 0x7F);
                     tracks[i]++;
                 } while (!track_delta[i]);
                 if ((!smallest_delta) || (smallest_delta > track_delta[i])) {
                     smallest_delta = track_delta[i];
                }
            NEXT_TRACK: continue;
            }

            subtract_delta = smallest_delta;
            sample_count_f = (((float) smallest_delta * samples_per_delta_f)
                              + sample_remainder);
            sample_count = (uint32_t) sample_count_f;
            sample_remainder = sample_count_f - (float) sample_count;

            mdi->events[mdi->event_count - 1].samples_to_next += sample_count;
            mdi->extra_info.approx_total_samples += sample_count;
        }
    } else {
        /* Type 0 & 2 */
        if (midi_type == 2) {
            mdi->is_type2 = 1;
        }
        sample_remainder = 0.0;
         for (i = 0; i < no_tracks; i++) {
             running_event[i] = 0;
             do {
                setup_ret = _WM_SetupMidiEvent(mdi, tracks[i], running_event[i]);
                 if (setup_ret == 0) {
                     goto _end;
                 }
                if (tracks[i][0] > 0x7f) {
                    if (tracks[i][0] < 0xf0) {
                        /* Events 0x80 - 0xef set running event */
                        running_event[i] = tracks[i][0];
                    } else if ((tracks[i][0] == 0xf0) || (tracks[i][0] == 0xf7)) {
                        /* Sysex resets running event */
                        running_event[i] = 0;
                    } else if ((tracks[i][0] == 0xff) && (tracks[i][1] == 0x2f) && (tracks[i][2] == 0x00)) {
                        /* End of Track */
                        track_end[i] = 1;
                        goto NEXT_TRACK2;
                    } else if ((tracks[i][0] == 0xff) && (tracks[i][1] == 0x51) && (tracks[i][2] == 0x03)) {
                        /* Tempo */
                        tempo = (tracks[i][3] << 16) + (tracks[i][4] << 8)+ tracks[i][5];
                        if (!tempo)
                            tempo = 500000;

                        samples_per_delta_f = _WM_GetSamplesPerTick(divisions, tempo);
                     }
                 }
                 tracks[i] += setup_ret;
 
                 track_delta[i] = 0;
                 if (*tracks[i] > 0x7f) {
                     do {
                         track_delta[i] = (track_delta[i] << 7) + (*tracks[i] & 0x7F);
                         tracks[i]++;
                     } while (*tracks[i] > 0x7f);
                 }
                 track_delta[i] = (track_delta[i] << 7) + (*tracks[i] & 0x7F);
                 tracks[i]++;
 
                 sample_count_f = (((float) track_delta[i] * samples_per_delta_f)
                                   + sample_remainder);
                sample_count = (uint32_t) sample_count_f;
                sample_remainder = sample_count_f - (float) sample_count;
                mdi->events[mdi->event_count - 1].samples_to_next += sample_count;
                mdi->extra_info.approx_total_samples += sample_count;
            NEXT_TRACK2:
                smallest_delta = track_delta[i]; /* Added just to keep Xcode happy */
                UNUSED(smallest_delta); /* Added to just keep clang happy */
            } while (track_end[i] == 0);
        }
    }

    if ((mdi->reverb = _WM_init_reverb(_WM_SampleRate, _WM_reverb_room_width,
            _WM_reverb_room_length, _WM_reverb_listen_posx, _WM_reverb_listen_posy))
          == NULL) {
        _WM_GLOBAL_ERROR(__FUNCTION__, __LINE__, WM_ERR_MEM, "to init reverb", 0);
        goto _end;
    }

    mdi->extra_info.current_sample = 0;
    mdi->current_event = &mdi->events[0];
    mdi->samples_to_mix = 0;
    mdi->note = NULL;

    _WM_ResetToStart(mdi);

_end:   free(sysex_store);
    free(track_end);
     free(track_delta);
     free(running_event);
     free(tracks);
     if (mdi->reverb) return (mdi);
     _WM_freeMDI(mdi);
     return (NULL);
}
