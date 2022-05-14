 create_response(const char *nurl, const char *method, unsigned int *rp_code)
 {
       char *page, *fpath;
        struct MHD_Response *resp = NULL;
 
        if (!strncmp(nurl, URL_BASE_API_1_1, strlen(URL_BASE_API_1_1))) {
                resp = create_response_api(nurl, method, rp_code);
        } else {
                fpath = get_path(nurl, server_data.www_dir);
 
               resp = create_response_file(nurl, method, rp_code, fpath);
 
                free(fpath);
        }
}
