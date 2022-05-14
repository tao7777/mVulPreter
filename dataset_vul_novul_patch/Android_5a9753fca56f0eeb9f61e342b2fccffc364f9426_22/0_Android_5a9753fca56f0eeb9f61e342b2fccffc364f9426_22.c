 int main(int argc, char **argv)
 {
    FILE                 *infile, *outfile[NUM_ENCODERS];
    FILE                 *downsampled_input[NUM_ENCODERS - 1];
    char                 filename[50];
     vpx_codec_ctx_t      codec[NUM_ENCODERS];
     vpx_codec_enc_cfg_t  cfg[NUM_ENCODERS];
    int                  frame_cnt = 0;
     vpx_image_t          raw[NUM_ENCODERS];
     vpx_codec_err_t      res[NUM_ENCODERS];
 
     int                  i;
     long                 width;
     long                 height;
    int                  length_frame;
     int                  frame_avail;
     int                  got_data;
     int                  flags = 0;
    int                  layer_id = 0;

    int                  layer_flags[VPX_TS_MAX_PERIODICITY * NUM_ENCODERS]
                                     = {0};
    int                  flag_periodicity;
 
     /*Currently, only realtime mode is supported in multi-resolution encoding.*/
     int                  arg_deadline = VPX_DL_REALTIME;

 /* Set show_psnr to 1/0 to show/not show PSNR. Choose show_psnr=0 if you

        don't need to know PSNR, which will skip PSNR calculation and save
        encoding time. */
     int                  show_psnr = 0;
    int                  key_frame_insert = 0;
     uint64_t             psnr_sse_total[NUM_ENCODERS] = {0};
     uint64_t             psnr_samples_total[NUM_ENCODERS] = {0};
     double               psnr_totals[NUM_ENCODERS][4] = {{0,0}};
     int                  psnr_count[NUM_ENCODERS] = {0};
 
    double               cx_time = 0;
    struct  timeval      tv1, tv2, difftv;

     /* Set the required target bitrates for each resolution level.
      * If target bitrate for highest-resolution level is set to 0,
      * (i.e. target_bitrate[0]=0), we skip encoding at that level.
      */
     unsigned int         target_bitrate[NUM_ENCODERS]={1000, 500, 100};

     /* Enter the frame rate of the input video */
     int                  framerate = 30;

     /* Set down-sampling factor for each resolution level.
        dsf[0] controls down sampling from level 0 to level 1;
        dsf[1] controls down sampling from level 1 to level 2;
        dsf[2] is not used. */
     vpx_rational_t dsf[NUM_ENCODERS] = {{2, 1}, {2, 1}, {1, 1}};
 
    /* Set the number of temporal layers for each encoder/resolution level,
     * starting from highest resoln down to lowest resoln. */
    unsigned int         num_temporal_layers[NUM_ENCODERS] = {3, 3, 3};

    if(argc!= (7 + 3 * NUM_ENCODERS))
        die("Usage: %s <width> <height> <frame_rate>  <infile> <outfile(s)> "
            "<rate_encoder(s)> <temporal_layer(s)> <key_frame_insert> <output psnr?> \n",
             argv[0]);
 
     printf("Using %s\n",vpx_codec_iface_name(interface));
 
     width = strtol(argv[1], NULL, 0);
     height = strtol(argv[2], NULL, 0);
    framerate = strtol(argv[3], NULL, 0);
 
     if(width < 16 || width%2 || height <16 || height%2)
         die("Invalid resolution: %ldx%ld", width, height);
 
     /* Open input video file for encoding */
    if(!(infile = fopen(argv[4], "rb")))
        die("Failed to open %s for reading", argv[4]);
 
     /* Open output file for each encoder to output bitstreams */
     for (i=0; i< NUM_ENCODERS; i++)
 {
 if(!target_bitrate[i])
 {
            outfile[i] = NULL;

             continue;
         }
 
        if(!(outfile[i] = fopen(argv[i+5], "wb")))
             die("Failed to open %s for writing", argv[i+4]);
     }
 
    // Bitrates per spatial layer: overwrite default rates above.
    for (i=0; i< NUM_ENCODERS; i++)
    {
        target_bitrate[i] = strtol(argv[NUM_ENCODERS + 5 + i], NULL, 0);
    }

    // Temporal layers per spatial layers: overwrite default settings above.
    for (i=0; i< NUM_ENCODERS; i++)
    {
        num_temporal_layers[i] = strtol(argv[2 * NUM_ENCODERS + 5 + i], NULL, 0);
        if (num_temporal_layers[i] < 1 || num_temporal_layers[i] > 3)
          die("Invalid temporal layers: %d, Must be 1, 2, or 3. \n",
              num_temporal_layers);
    }

    /* Open file to write out each spatially downsampled input stream. */
    for (i=0; i< NUM_ENCODERS - 1; i++)
    {
       // Highest resoln is encoder 0.
        if (sprintf(filename,"ds%d.yuv",NUM_ENCODERS - i) < 0)
        {
            return EXIT_FAILURE;
        }
        downsampled_input[i] = fopen(filename,"wb");
    }

    key_frame_insert = strtol(argv[3 * NUM_ENCODERS + 5], NULL, 0);

    show_psnr = strtol(argv[3 * NUM_ENCODERS + 6], NULL, 0);

 
     /* Populate default encoder configuration */
     for (i=0; i< NUM_ENCODERS; i++)
 {
        res[i] = vpx_codec_enc_config_default(interface, &cfg[i], 0);
 if(res[i]) {
            printf("Failed to get config: %s\n", vpx_codec_err_to_string(res[i]));
 return EXIT_FAILURE;
 }
 }

 /*
     * Update the default configuration according to needs of the application.
     */

     /* Highest-resolution encoder settings */
     cfg[0].g_w = width;
     cfg[0].g_h = height;
    cfg[0].rc_dropframe_thresh = 0;
     cfg[0].rc_end_usage = VPX_CBR;
     cfg[0].rc_resize_allowed = 0;
    cfg[0].rc_min_quantizer = 2;
     cfg[0].rc_max_quantizer = 56;
    cfg[0].rc_undershoot_pct = 100;
    cfg[0].rc_overshoot_pct = 15;
     cfg[0].rc_buf_initial_sz = 500;
     cfg[0].rc_buf_optimal_sz = 600;
     cfg[0].rc_buf_sz = 1000;
    cfg[0].g_error_resilient = 1; /* Enable error resilient mode */
    cfg[0].g_lag_in_frames   = 0;

 /* Disable automatic keyframe placement */

     /* Note: These 3 settings are copied to all levels. But, except the lowest
      * resolution level, all other levels are set to VPX_KF_DISABLED internally.
      */
     cfg[0].kf_min_dist = 3000;
     cfg[0].kf_max_dist = 3000;

    cfg[0].rc_target_bitrate = target_bitrate[0]; /* Set target bitrate */
    cfg[0].g_timebase.num = 1; /* Set fps */
    cfg[0].g_timebase.den = framerate;

 /* Other-resolution encoder settings */
 for (i=1; i< NUM_ENCODERS; i++)

     {
         memcpy(&cfg[i], &cfg[0], sizeof(vpx_codec_enc_cfg_t));
 
         cfg[i].rc_target_bitrate = target_bitrate[i];
 
         /* Note: Width & height of other-resolution encoders are calculated
         * from the highest-resolution encoder's size and the corresponding
         * down_sampling_factor.
         */
 {
 unsigned int iw = cfg[i-1].g_w*dsf[i-1].den + dsf[i-1].num - 1;
 unsigned int ih = cfg[i-1].g_h*dsf[i-1].den + dsf[i-1].num - 1;
            cfg[i].g_w = iw/dsf[i-1].num;
            cfg[i].g_h = ih/dsf[i-1].num;
 }

 /* Make width & height to be multiplier of 2. */
 if((cfg[i].g_w)%2)cfg[i].g_w++;

         if((cfg[i].g_h)%2)cfg[i].g_h++;
     }
 

    // Set the number of threads per encode/spatial layer.
    // (1, 1, 1) means no encoder threading.
    cfg[0].g_threads = 2;
    cfg[1].g_threads = 1;
    cfg[2].g_threads = 1;

     /* Allocate image for each encoder */
     for (i=0; i< NUM_ENCODERS; i++)
         if(!vpx_img_alloc(&raw[i], VPX_IMG_FMT_I420, cfg[i].g_w, cfg[i].g_h, 32))
            die("Failed to allocate image", cfg[i].g_w, cfg[i].g_h);

 if (raw[0].stride[VPX_PLANE_Y] == raw[0].d_w)
        read_frame_p = read_frame;
 else
        read_frame_p = read_frame_by_row;

 for (i=0; i< NUM_ENCODERS; i++)

         if(outfile[i])
             write_ivf_file_header(outfile[i], &cfg[i], 0);
 
    /* Temporal layers settings */
    for ( i=0; i<NUM_ENCODERS; i++)
    {
        set_temporal_layer_pattern(num_temporal_layers[i],
                                   &cfg[i],
                                   cfg[i].rc_target_bitrate,
                                   &layer_flags[i * VPX_TS_MAX_PERIODICITY]);
    }

     /* Initialize multi-encoder */
     if(vpx_codec_enc_init_multi(&codec[0], interface, &cfg[0], NUM_ENCODERS,
                                 (show_psnr ? VPX_CODEC_USE_PSNR : 0), &dsf[0]))
        die_codec(&codec[0], "Failed to initialize encoder");

 /* The extra encoding configuration parameters can be set as follows. */
 /* Set encoding speed */

     for ( i=0; i<NUM_ENCODERS; i++)
     {
         int speed = -6;
        /* Lower speed for the lowest resolution. */
        if (i == NUM_ENCODERS - 1) speed = -4;
         if(vpx_codec_control(&codec[i], VP8E_SET_CPUUSED, speed))
             die_codec(&codec[i], "Failed to set cpu_used");
     }
 
    /* Set static threshold = 1 for all encoders */
     for ( i=0; i<NUM_ENCODERS; i++)
     {
        if(vpx_codec_control(&codec[i], VP8E_SET_STATIC_THRESHOLD, 1))
             die_codec(&codec[i], "Failed to set static threshold");
     }
 
 /* Set NOISE_SENSITIVITY to do TEMPORAL_DENOISING */
 /* Enable denoising for the highest-resolution encoder. */
 if(vpx_codec_control(&codec[0], VP8E_SET_NOISE_SENSITIVITY, 1))
        die_codec(&codec[0], "Failed to set noise_sensitivity");
 for ( i=1; i< NUM_ENCODERS; i++)
 {
 if(vpx_codec_control(&codec[i], VP8E_SET_NOISE_SENSITIVITY, 0))

             die_codec(&codec[i], "Failed to set noise_sensitivity");
     }
 
    /* Set the number of token partitions */
    for ( i=0; i<NUM_ENCODERS; i++)
    {
        if(vpx_codec_control(&codec[i], VP8E_SET_TOKEN_PARTITIONS, 1))
            die_codec(&codec[i], "Failed to set static threshold");
    }

    /* Set the max intra target bitrate */
    for ( i=0; i<NUM_ENCODERS; i++)
    {
        unsigned int max_intra_size_pct =
            (int)(((double)cfg[0].rc_buf_optimal_sz * 0.5) * framerate / 10);
        if(vpx_codec_control(&codec[i], VP8E_SET_MAX_INTRA_BITRATE_PCT,
                             max_intra_size_pct))
            die_codec(&codec[i], "Failed to set static threshold");
       //printf("%d %d \n",i,max_intra_size_pct);
    }
 
     frame_avail = 1;
     got_data = 0;

 while(frame_avail || got_data)
 {
 vpx_codec_iter_t iter[NUM_ENCODERS]={NULL};
 const vpx_codec_cx_pkt_t *pkt[NUM_ENCODERS];

        flags = 0;
        frame_avail = read_frame_p(infile, &raw[0]);

 if(frame_avail)
 {
 for ( i=1; i<NUM_ENCODERS; i++)
 {
 /*Scale the image down a number of times by downsampling factor*/
 /* FilterMode 1 or 2 give better psnr than FilterMode 0. */
                I420Scale(raw[i-1].planes[VPX_PLANE_Y], raw[i-1].stride[VPX_PLANE_Y],
                          raw[i-1].planes[VPX_PLANE_U], raw[i-1].stride[VPX_PLANE_U],
                          raw[i-1].planes[VPX_PLANE_V], raw[i-1].stride[VPX_PLANE_V],
                          raw[i-1].d_w, raw[i-1].d_h,
                          raw[i].planes[VPX_PLANE_Y], raw[i].stride[VPX_PLANE_Y],

                           raw[i].planes[VPX_PLANE_U], raw[i].stride[VPX_PLANE_U],
                           raw[i].planes[VPX_PLANE_V], raw[i].stride[VPX_PLANE_V],
                           raw[i].d_w, raw[i].d_h, 1);
                /* Write out down-sampled input. */
                length_frame = cfg[i].g_w *  cfg[i].g_h *3/2;
                if (fwrite(raw[i].planes[0], 1, length_frame,
                           downsampled_input[NUM_ENCODERS - i - 1]) !=
                               length_frame)
                {
                    return EXIT_FAILURE;
                }
             }
         }
 
        /* Set the flags (reference and update) for all the encoders.*/
        for ( i=0; i<NUM_ENCODERS; i++)
        {
            layer_id = cfg[i].ts_layer_id[frame_cnt % cfg[i].ts_periodicity];
            flags = 0;
            flag_periodicity = periodicity_to_num_layers
                [num_temporal_layers[i] - 1];
            flags = layer_flags[i * VPX_TS_MAX_PERIODICITY +
                                frame_cnt % flag_periodicity];
            // Key frame flag for first frame.
            if (frame_cnt == 0)
            {
                flags |= VPX_EFLAG_FORCE_KF;
            }
            if (frame_cnt > 0 && frame_cnt == key_frame_insert)
            {
                flags = VPX_EFLAG_FORCE_KF;
            }
 
            vpx_codec_control(&codec[i], VP8E_SET_FRAME_FLAGS, flags);
            vpx_codec_control(&codec[i], VP8E_SET_TEMPORAL_LAYER_ID, layer_id);
        }

        gettimeofday(&tv1, NULL);
        /* Encode each frame at multi-levels */
        /* Note the flags must be set to 0 in the encode call if they are set
           for each frame with the vpx_codec_control(), as done above. */
        if(vpx_codec_encode(&codec[0], frame_avail? &raw[0] : NULL,
            frame_cnt, 1, 0, arg_deadline))
        {
            die_codec(&codec[0], "Failed to encode frame");
        }
        gettimeofday(&tv2, NULL);
        timersub(&tv2, &tv1, &difftv);
        cx_time += (double)(difftv.tv_sec * 1000000 + difftv.tv_usec);
         for (i=NUM_ENCODERS-1; i>=0 ; i--)
         {
             got_data = 0;
             while( (pkt[i] = vpx_codec_get_cx_data(&codec[i], &iter[i])) )
             {
                 got_data = 1;
 switch(pkt[i]->kind) {
 case VPX_CODEC_CX_FRAME_PKT:
                        write_ivf_frame_header(outfile[i], pkt[i]);
 (void) fwrite(pkt[i]->data.frame.buf, 1,
                                      pkt[i]->data.frame.sz, outfile[i]);
 break;
 case VPX_CODEC_PSNR_PKT:
 if (show_psnr)
 {
 int j;

                            psnr_sse_total[i] += pkt[i]->data.psnr.sse[0];

                             psnr_samples_total[i] += pkt[i]->data.psnr.samples[0];
                             for (j = 0; j < 4; j++)
                             {
                             }
                             psnr_count[i]++;
 }

 break;
 default:

                         break;
                 }
                 printf(pkt[i]->kind == VPX_CODEC_CX_FRAME_PKT
                       && (pkt[i]->data.frame.flags & VPX_FRAME_IS_KEY)? "K":"");
                 fflush(stdout);
             }
         }
         frame_cnt++;
     }
     printf("\n");
    printf("FPS for encoding %d %f %f \n", frame_cnt, (float)cx_time / 1000000,
           1000000 * (double)frame_cnt / (double)cx_time);
 
     fclose(infile);
 
    printf("Processed %ld frames.\n",(long int)frame_cnt-1);
 for (i=0; i< NUM_ENCODERS; i++)
 {
 /* Calculate PSNR and print it out */
 if ( (show_psnr) && (psnr_count[i]>0) )
 {
 int j;
 double ovpsnr = sse_to_psnr(psnr_samples_total[i], 255.0,
                                        psnr_sse_total[i]);

            fprintf(stderr, "\n ENC%d PSNR (Overall/Avg/Y/U/V)", i);

            fprintf(stderr, " %.3lf", ovpsnr);
 for (j = 0; j < 4; j++)
 {
                fprintf(stderr, " %.3lf", psnr_totals[i][j]/psnr_count[i]);
 }
 }

 if(vpx_codec_destroy(&codec[i]))
            die_codec(&codec[i], "Failed to destroy codec");

        vpx_img_free(&raw[i]);

 if(!outfile[i])
 continue;

 /* Try to rewrite the file header with the actual frame count */
 if(!fseek(outfile[i], 0, SEEK_SET))
            write_ivf_file_header(outfile[i], &cfg[i], frame_cnt-1);
        fclose(outfile[i]);
 }
    printf("\n");

 return EXIT_SUCCESS;
}
