fd_read_body (const char *downloaded_filename, int fd, FILE *out, wgint toread, wgint startpos,

              wgint *qtyread, wgint *qtywritten, double *elapsed, int flags,
              FILE *out2)
{
  int ret = 0;
#undef max
#define max(a,b) ((a) > (b) ? (a) : (b))
  int dlbufsize = max (BUFSIZ, 8 * 1024);
  char *dlbuf = xmalloc (dlbufsize);

  struct ptimer *timer = NULL;
  double last_successful_read_tm = 0;

  /* The progress gauge, set according to the user preferences. */
  void *progress = NULL;

  /* Non-zero if the progress gauge is interactive, i.e. if it can
     continually update the display.  When true, smaller timeout
     values are used so that the gauge can update the display when
     data arrives slowly. */
  bool progress_interactive = false;

  bool exact = !!(flags & rb_read_exactly);

  /* Used only by HTTP/HTTPS chunked transfer encoding.  */
  bool chunked = flags & rb_chunked_transfer_encoding;
  wgint skip = 0;

  /* How much data we've read/written.  */
  wgint sum_read = 0;
  wgint sum_written = 0;
  wgint remaining_chunk_size = 0;

#ifdef HAVE_LIBZ
  /* try to minimize the number of calls to inflate() and write_data() per
     call to fd_read() */
  unsigned int gzbufsize = dlbufsize * 4;
  char *gzbuf = NULL;
  z_stream gzstream;

  if (flags & rb_compressed_gzip)
    {
      gzbuf = xmalloc (gzbufsize);
      if (gzbuf != NULL)
        {
          gzstream.zalloc = zalloc;
          gzstream.zfree = zfree;
          gzstream.opaque = Z_NULL;
          gzstream.next_in = Z_NULL;
          gzstream.avail_in = 0;

          #define GZIP_DETECT 32 /* gzip format detection */
          #define GZIP_WINDOW 15 /* logarithmic window size (default: 15) */
          ret = inflateInit2 (&gzstream, GZIP_DETECT | GZIP_WINDOW);
          if (ret != Z_OK)
            {
              xfree (gzbuf);
              errno = (ret == Z_MEM_ERROR) ? ENOMEM : EINVAL;
              ret = -1;
              goto out;
            }
        }
      else
        {
          errno = ENOMEM;
          ret = -1;
          goto out;
        }
    }
#endif

  if (flags & rb_skip_startpos)
    skip = startpos;

  if (opt.show_progress)
    {
      const char *filename_progress;
      /* If we're skipping STARTPOS bytes, pass 0 as the INITIAL
         argument to progress_create because the indicator doesn't
         (yet) know about "skipping" data.  */
      wgint start = skip ? 0 : startpos;
      if (opt.dir_prefix)
        filename_progress = downloaded_filename + strlen (opt.dir_prefix) + 1;
      else
        filename_progress = downloaded_filename;
      progress = progress_create (filename_progress, start, start + toread);
      progress_interactive = progress_interactive_p (progress);
    }

  if (opt.limit_rate)
    limit_bandwidth_reset ();

  /* A timer is needed for tracking progress, for throttling, and for
     tracking elapsed time.  If either of these are requested, start
     the timer.  */
  if (progress || opt.limit_rate || elapsed)
    {
      timer = ptimer_new ();
      last_successful_read_tm = 0;
    }

  /* Use a smaller buffer for low requested bandwidths.  For example,
     with --limit-rate=2k, it doesn't make sense to slurp in 16K of
     data and then sleep for 8s.  With buffer size equal to the limit,
     we never have to sleep for more than one second.  */
  if (opt.limit_rate && opt.limit_rate < dlbufsize)
    dlbufsize = opt.limit_rate;

  /* Read from FD while there is data to read.  Normally toread==0
     means that it is unknown how much data is to arrive.  However, if
     EXACT is set, then toread==0 means what it says: that no data
     should be read.  */
  while (!exact || (sum_read < toread))
    {
      int rdsize;
      double tmout = opt.read_timeout;

      if (chunked)
        {
          if (remaining_chunk_size == 0)
            {
              char *line = fd_read_line (fd);
              char *endl;
              if (line == NULL)
                {
                  ret = -1;
                  break;
                }
              else if (out2 != NULL)
                fwrite (line, 1, strlen (line), out2);

               remaining_chunk_size = strtol (line, &endl, 16);
               xfree (line);
 
              if (remaining_chunk_size < 0)
                {
                  ret = -1;
                  break;
                }

               if (remaining_chunk_size == 0)
                 {
                   ret = 0;
                        fwrite (line, 1, strlen (line), out2);
                      xfree (line);
                    }
                  break;
                }
            }

          rdsize = MIN (remaining_chunk_size, dlbufsize);
        }
      else
        rdsize = exact ? MIN (toread - sum_read, dlbufsize) : dlbufsize;

      if (progress_interactive)
        {
          /* For interactive progress gauges, always specify a ~1s
             timeout, so that the gauge can be updated regularly even
             when the data arrives very slowly or stalls.  */
          tmout = 0.95;
          if (opt.read_timeout)
            {
              double waittm;
              waittm = ptimer_read (timer) - last_successful_read_tm;
              if (waittm + tmout > opt.read_timeout)
                {
                  /* Don't let total idle time exceed read timeout. */
                  tmout = opt.read_timeout - waittm;
                  if (tmout < 0)
                    {
                      /* We've already exceeded the timeout. */
                      ret = -1, errno = ETIMEDOUT;
                      break;
                    }
                }
            }
        }
      ret = fd_read (fd, dlbuf, rdsize, tmout);

      if (progress_interactive && ret < 0 && errno == ETIMEDOUT)
        ret = 0;                /* interactive timeout, handled above */
      else if (ret <= 0)
        break;                  /* EOF or read error */

      if (progress || opt.limit_rate || elapsed)
        {
          ptimer_measure (timer);
          if (ret > 0)
            last_successful_read_tm = ptimer_read (timer);
        }

      if (ret > 0)
        {
          int write_res;

          sum_read += ret;

#ifdef HAVE_LIBZ
          if (gzbuf != NULL)
            {
              int err;
              int towrite;
              gzstream.avail_in = ret;
              gzstream.next_in = (unsigned char *) dlbuf;

              do
                {
                  gzstream.avail_out = gzbufsize;
                  gzstream.next_out = (unsigned char *) gzbuf;

                  err = inflate (&gzstream, Z_NO_FLUSH);

                  switch (err)
                    {
                    case Z_MEM_ERROR:
                      errno = ENOMEM;
                      ret = -1;
                      goto out;
                    case Z_NEED_DICT:
                    case Z_DATA_ERROR:
                      errno = EINVAL;
                      ret = -1;
                      goto out;
                    case Z_STREAM_END:
                      if (exact && sum_read != toread)
                        {
                          DEBUGP(("zlib stream ended unexpectedly after "
                                  "%ld/%ld bytes\n", sum_read, toread));
                        }
                    }

                  towrite = gzbufsize - gzstream.avail_out;
                  write_res = write_data (out, out2, gzbuf, towrite, &skip,
                                          &sum_written);
                  if (write_res < 0)
                    {
                      ret = (write_res == -3) ? -3 : -2;
                      goto out;
                    }
                }
              while (gzstream.avail_out == 0);
            }
          else
#endif
            {
              write_res = write_data (out, out2, dlbuf, ret, &skip,
                                      &sum_written);
              if (write_res < 0)
                {
                  ret = (write_res == -3) ? -3 : -2;
                  goto out;
                }
            }

          if (chunked)
            {
              remaining_chunk_size -= ret;
              if (remaining_chunk_size == 0)
                {
                  char *line = fd_read_line (fd);
                  if (line == NULL)
                    {
                      ret = -1;
                      break;
                    }
                  else
                    {
                      if (out2 != NULL)
                        fwrite (line, 1, strlen (line), out2);
                      xfree (line);
                    }
                }
            }
        }

      if (opt.limit_rate)
        limit_bandwidth (ret, timer);

      if (progress)
        progress_update (progress, ret, ptimer_read (timer));
#ifdef WINDOWS
      if (toread > 0 && opt.show_progress)
        ws_percenttitle (100.0 *
                         (startpos + sum_read) / (startpos + toread));
#endif
    }
