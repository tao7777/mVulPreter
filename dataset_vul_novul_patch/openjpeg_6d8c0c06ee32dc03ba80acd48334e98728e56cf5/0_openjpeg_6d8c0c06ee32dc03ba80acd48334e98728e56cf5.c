int main(int argc, char *argv[])
{
    opj_dinfo_t* dinfo;
    opj_event_mgr_t event_mgr;      /* event manager */
    int tnum;
    unsigned int snum;
    opj_mj2_t *movie;
    mj2_tk_t *track;
    mj2_sample_t *sample;
    unsigned char* frame_codestream;
    FILE *file, *outfile;
    char outfilename[50];
    mj2_dparameters_t parameters;

    if (argc != 3) {
        printf("Usage: %s mj2filename output_location\n", argv[0]);
        printf("Example: %s foreman.mj2 output/foreman\n", argv[0]);
        return 1;
    }

    file = fopen(argv[1], "rb");

    if (!file) {
        fprintf(stderr, "failed to open %s for reading\n", argv[1]);
        return 1;
    }

    /*
    configure the event callbacks (not required)
    setting of each callback is optional
    */
    memset(&event_mgr, 0, sizeof(opj_event_mgr_t));
    event_mgr.error_handler = error_callback;
    event_mgr.warning_handler = warning_callback;
    event_mgr.info_handler = info_callback;

    /* get a MJ2 decompressor handle */
    dinfo = mj2_create_decompress();

    /* catch events using our callbacks and give a local context */
    opj_set_event_mgr((opj_common_ptr)dinfo, &event_mgr, stderr);

    /* setup the decoder decoding parameters using user parameters */
    memset(&parameters, 0, sizeof(mj2_dparameters_t));
    movie = (opj_mj2_t*) dinfo->mj2_handle;
    mj2_setup_decoder(movie, &parameters);

    if (mj2_read_struct(file, movie)) { /* Creating the movie structure*/
        return 1;
    }

    /* Decode first video track */
    tnum = 0;
    while (movie->tk[tnum].track_type != 0) {
        tnum ++;
    }

    track = &movie->tk[tnum];

    fprintf(stdout, "Extracting %d frames from file...\n", track->num_samples);

    for (snum = 0; snum < track->num_samples; snum++) {
        sample = &track->sample[snum];
        frame_codestream = (unsigned char*) malloc(sample->sample_size -
                           8); /* Skipping JP2C marker*/
        fseek(file, sample->offset + 8, SEEK_SET);
         fread(frame_codestream, sample->sample_size - 8, 1,
               file); /* Assuming that jp and ftyp markers size do*/
 
        int num = snprintf(outfilename, sizeof(outfilename), "%s_%05d.j2k", argv[2], snum);
        if (num >= sizeof(outfilename)) {
            fprintf(stderr, "maximum length of output prefix exceeded\n");
            return 1;
        }

         outfile = fopen(outfilename, "wb");
         if (!outfile) {
             fprintf(stderr, "failed to open %s for writing\n", outfilename);
            return 1;
        }
        fwrite(frame_codestream, sample->sample_size - 8, 1, outfile);
        fclose(outfile);
        free(frame_codestream);
    }
    fclose(file);
    fprintf(stdout, "%d frames correctly extracted\n", snum);

    /* free remaining structures */
    if (dinfo) {
        mj2_destroy_decompress((opj_mj2_t*)dinfo->mj2_handle);
    }

    return 0;
}
