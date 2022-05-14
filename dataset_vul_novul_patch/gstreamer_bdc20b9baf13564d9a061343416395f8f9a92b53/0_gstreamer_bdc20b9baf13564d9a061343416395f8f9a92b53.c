qtdemux_parse_samples (GstQTDemux * qtdemux, QtDemuxStream * stream,
    GNode * stbl)
{
  int offset;
  GNode *stsc;
  GNode *stsz;
  GNode *stco;
  GNode *co64;
  GNode *stts;
  GNode *stss;
  GNode *ctts;
  const guint8 *stsc_data, *stsz_data, *stco_data;
  int sample_size;
  int sample_index;
  int n_samples;
  int n_samples_per_chunk;
  int n_sample_times;
  QtDemuxSample *samples;
  gint i, j, k;
  int index;
  guint64 timestamp, time;

  /* sample to chunk */
  if (!(stsc = qtdemux_tree_get_child_by_type (stbl, FOURCC_stsc)))
    goto corrupt_file;
  stsc_data = (const guint8 *) stsc->data;
  /* sample size */
  if (!(stsz = qtdemux_tree_get_child_by_type (stbl, FOURCC_stsz)))
    goto corrupt_file;
  stsz_data = (const guint8 *) stsz->data;
  /* chunk offsets */
  stco = qtdemux_tree_get_child_by_type (stbl, FOURCC_stco);
  co64 = qtdemux_tree_get_child_by_type (stbl, FOURCC_co64);
  if (stco) {
    stco_data = (const guint8 *) stco->data;
  } else {
    stco_data = NULL;
    if (co64 == NULL)
      goto corrupt_file;
  }
  /* sample time */
  if (!(stts = qtdemux_tree_get_child_by_type (stbl, FOURCC_stts)))
    goto corrupt_file;

  /* sample sync, can be NULL */
  stss = qtdemux_tree_get_child_by_type (stbl, FOURCC_stss);

  sample_size = QT_UINT32 (stsz_data + 12);
  if (sample_size == 0 || stream->sampled) {
    n_samples = QT_UINT32 (stsz_data + 16);
    GST_DEBUG_OBJECT (qtdemux, "stsz sample_size 0, allocating n_samples %d",
        n_samples);
    stream->n_samples = n_samples;
    samples = g_new0 (QtDemuxSample, n_samples);
    stream->samples = samples;

    for (i = 0; i < n_samples; i++) {
      if (sample_size == 0)
        samples[i].size = QT_UINT32 (stsz_data + i * 4 + 20);
      else
        samples[i].size = sample_size;

      GST_LOG_OBJECT (qtdemux, "sample %d has size %d", i, samples[i].size);
      /* init other fields to defaults for this sample */
      samples[i].keyframe = FALSE;
    }
    n_samples_per_chunk = QT_UINT32 (stsc_data + 12);
    index = 0;
    for (i = 0; i < n_samples_per_chunk; i++) {
      guint32 first_chunk, last_chunk;
      guint32 samples_per_chunk;

      first_chunk = QT_UINT32 (stsc_data + 16 + i * 12 + 0) - 1;
      if (i == n_samples_per_chunk - 1) {
        last_chunk = G_MAXUINT32;
      } else {
        last_chunk = QT_UINT32 (stsc_data + 16 + i * 12 + 12) - 1;
      }
      samples_per_chunk = QT_UINT32 (stsc_data + 16 + i * 12 + 4);

      for (j = first_chunk; j < last_chunk; j++) {
        guint64 chunk_offset;

        if (stco) {
          chunk_offset = QT_UINT32 (stco_data + 16 + j * 4);
        } else {
          chunk_offset = QT_UINT64 ((guint8 *) co64->data + 16 + j * 8);
        }
        for (k = 0; k < samples_per_chunk; k++) {
          GST_LOG_OBJECT (qtdemux, "Creating entry %d with offset %lld",
              index, chunk_offset);
          samples[index].chunk = j;
          samples[index].offset = chunk_offset;
          chunk_offset += samples[index].size;
          index++;
          if (index >= n_samples)
            goto done2;
        }
      }
    }
  done2:

    n_sample_times = QT_UINT32 ((guint8 *) stts->data + 12);
    timestamp = 0;
     stream->min_duration = 0;
     time = 0;
     index = 0;
    for (i = 0; (i < n_sample_times) && (index < stream->n_samples); i++) {
       guint32 n;
       guint32 duration;
 
       n = QT_UINT32 ((guint8 *) stts->data + 16 + 8 * i);
       duration = QT_UINT32 ((guint8 *) stts->data + 16 + 8 * i + 4);
      for (j = 0; (j < n) && (index < stream->n_samples); j++) {
         GST_DEBUG_OBJECT (qtdemux, "sample %d: timestamp %" GST_TIME_FORMAT,
             index, GST_TIME_ARGS (timestamp));
 
        samples[index].timestamp = timestamp;
        /* take first duration for fps */
        if (stream->min_duration == 0)
          stream->min_duration = duration;
        /* add non-scaled values to avoid rounding errors */
        time += duration;
        timestamp = gst_util_uint64_scale (time, GST_SECOND, stream->timescale);
        samples[index].duration = timestamp - samples[index].timestamp;

        index++;
      }
    }
    if (stss) {
      /* mark keyframes */
      guint32 n_sample_syncs;

      n_sample_syncs = QT_UINT32 ((guint8 *) stss->data + 12);
      if (n_sample_syncs == 0) {
        stream->all_keyframe = TRUE;
      } else {
        offset = 16;
         for (i = 0; i < n_sample_syncs; i++) {
           /* note that the first sample is index 1, not 0 */
           index = QT_UINT32 ((guint8 *) stss->data + offset);
          if (index > 0 && index <= stream->n_samples) {
             samples[index - 1].keyframe = TRUE;
             offset += 4;
           }
        }
      }
    } else {
      /* no stss, all samples are keyframes */
      stream->all_keyframe = TRUE;
    }
  } else {
    GST_DEBUG_OBJECT (qtdemux,
        "stsz sample_size %d != 0, treating chunks as samples", sample_size);

    /* treat chunks as samples */
    if (stco) {
      n_samples = QT_UINT32 (stco_data + 12);
    } else {
      n_samples = QT_UINT32 ((guint8 *) co64->data + 12);
    }
    stream->n_samples = n_samples;
    GST_DEBUG_OBJECT (qtdemux, "allocating n_samples %d", n_samples);
    samples = g_new0 (QtDemuxSample, n_samples);
    stream->samples = samples;

    n_samples_per_chunk = QT_UINT32 (stsc_data + 12);
    GST_DEBUG_OBJECT (qtdemux, "n_samples_per_chunk %d", n_samples_per_chunk);
    sample_index = 0;
    timestamp = 0;
    for (i = 0; i < n_samples_per_chunk; i++) {
      guint32 first_chunk, last_chunk;
      guint32 samples_per_chunk;

      first_chunk = QT_UINT32 (stsc_data + 16 + i * 12 + 0) - 1;
      /* the last chunk of each entry is calculated by taking the first chunk
       * of the next entry; except if there is no next, where we fake it with
       * INT_MAX */
      if (i == n_samples_per_chunk - 1) {
        last_chunk = G_MAXUINT32;
      } else {
        last_chunk = QT_UINT32 (stsc_data + 16 + i * 12 + 12) - 1;
      }
      samples_per_chunk = QT_UINT32 (stsc_data + 16 + i * 12 + 4);

      GST_LOG_OBJECT (qtdemux,
          "entry %d has first_chunk %d, last_chunk %d, samples_per_chunk %d", i,
          first_chunk, last_chunk, samples_per_chunk);

      for (j = first_chunk; j < last_chunk; j++) {
        guint64 chunk_offset;

        if (j >= n_samples)
          goto done;

        if (stco) {
          chunk_offset = QT_UINT32 (stco_data + 16 + j * 4);
        } else {
          chunk_offset = QT_UINT64 ((guint8 *) co64->data + 16 + j * 8);
        }
        GST_LOG_OBJECT (qtdemux,
            "Creating entry %d with offset %" G_GUINT64_FORMAT, j,
            chunk_offset);

        samples[j].chunk = j;
        samples[j].offset = chunk_offset;

        if (stream->samples_per_frame * stream->bytes_per_frame) {
          samples[j].size = (samples_per_chunk * stream->n_channels) /
              stream->samples_per_frame * stream->bytes_per_frame;
        } else {
          samples[j].size = samples_per_chunk;
        }

        GST_DEBUG_OBJECT (qtdemux, "sample %d: timestamp %" GST_TIME_FORMAT
            ", size %u", j, GST_TIME_ARGS (timestamp), samples[j].size);

        samples[j].timestamp = timestamp;
        sample_index += samples_per_chunk;

        timestamp = gst_util_uint64_scale (sample_index,
            GST_SECOND, stream->timescale);
        samples[j].duration = timestamp - samples[j].timestamp;

        samples[j].keyframe = TRUE;
      }
    }
  }

  /* composition time to sample */
  if ((ctts = qtdemux_tree_get_child_by_type (stbl, FOURCC_ctts))) {
    const guint8 *ctts_data = (const guint8 *) ctts->data;
    guint32 n_entries = QT_UINT32 (ctts_data + 12);
    guint32 count;
    gint32 soffset;

    /* Fill in the pts_offsets */
     for (i = 0, j = 0; (j < stream->n_samples) && (i < n_entries); i++) {
       count = QT_UINT32 (ctts_data + 16 + i * 8);
       soffset = QT_UINT32 (ctts_data + 20 + i * 8);
      for (k = 0; (k < count) && (j < stream->n_samples); k++, j++) {
         /* we operate with very small soffset values here, it shouldn't overflow */
         samples[j].pts_offset = soffset * GST_SECOND / stream->timescale;
       }
    }
  }
done:
  return TRUE;

/* ERRORS */
corrupt_file:
  {
    GST_ELEMENT_ERROR (qtdemux, STREAM, DECODE,
        (_("This file is corrupt and cannot be played.")), (NULL));
    return FALSE;
  }
}
