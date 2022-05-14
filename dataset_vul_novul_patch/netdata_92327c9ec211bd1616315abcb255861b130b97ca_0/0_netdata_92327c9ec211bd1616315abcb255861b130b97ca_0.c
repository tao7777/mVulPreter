 inline int web_client_api_request_v1_data(RRDHOST *host, struct web_client *w, char *url) {
     debug(D_WEB_CLIENT, "%llu: API v1 data with URL '%s'", w->id, url);

    int ret = 400;
    BUFFER *dimensions = NULL;

    buffer_flush(w->response.data);

    char    *google_version = "0.6",
            *google_reqId = "0",
            *google_sig = "0",
            *google_out = "json",
            *responseHandler = NULL,
            *outFileName = NULL;

    time_t last_timestamp_in_data = 0, google_timestamp = 0;

    char *chart = NULL
    , *before_str = NULL
    , *after_str = NULL
    , *group_time_str = NULL
    , *points_str = NULL;

    int group = RRDR_GROUPING_AVERAGE;
    uint32_t format = DATASOURCE_JSON;
    uint32_t options = 0x00000000;

    while(url) {
        char *value = mystrsep(&url, "?&");
        if(!value || !*value) continue;

        char *name = mystrsep(&value, "=");
        if(!name || !*name) continue;
        if(!value || !*value) continue;

        debug(D_WEB_CLIENT, "%llu: API v1 data query param '%s' with value '%s'", w->id, name, value);


        if(!strcmp(name, "chart")) chart = value;
        else if(!strcmp(name, "dimension") || !strcmp(name, "dim") || !strcmp(name, "dimensions") || !strcmp(name, "dims")) {
            if(!dimensions) dimensions = buffer_create(100);
            buffer_strcat(dimensions, "|");
            buffer_strcat(dimensions, value);
        }
        else if(!strcmp(name, "after")) after_str = value;
        else if(!strcmp(name, "before")) before_str = value;
        else if(!strcmp(name, "points")) points_str = value;
        else if(!strcmp(name, "gtime")) group_time_str = value;
        else if(!strcmp(name, "group")) {
            group = web_client_api_request_v1_data_group(value, RRDR_GROUPING_AVERAGE);
        }
        else if(!strcmp(name, "format")) {
            format = web_client_api_request_v1_data_format(value);
        }
        else if(!strcmp(name, "options")) {
            options |= web_client_api_request_v1_data_options(value);
        }
        else if(!strcmp(name, "callback")) {
            responseHandler = value;
        }
        else if(!strcmp(name, "filename")) {
            outFileName = value;
        }
        else if(!strcmp(name, "tqx")) {
            char *tqx_name, *tqx_value;

            while(value) {
                tqx_value = mystrsep(&value, ";");
                if(!tqx_value || !*tqx_value) continue;

                tqx_name = mystrsep(&tqx_value, ":");
                if(!tqx_name || !*tqx_name) continue;
                if(!tqx_value || !*tqx_value) continue;

                if(!strcmp(tqx_name, "version"))
                    google_version = tqx_value;
                else if(!strcmp(tqx_name, "reqId"))
                    google_reqId = tqx_value;
                else if(!strcmp(tqx_name, "sig")) {
                    google_sig = tqx_value;
                    google_timestamp = strtoul(google_sig, NULL, 0);
                }
                else if(!strcmp(tqx_name, "out")) {
                    google_out = tqx_value;
                    format = web_client_api_request_v1_data_google_format(google_out);
                }
                else if(!strcmp(tqx_name, "responseHandler"))
                    responseHandler = tqx_value;
                else if(!strcmp(tqx_name, "outFileName"))
                    outFileName = tqx_value;
            }
         }
     }
 
    // validate the google parameters given
    fix_google_param(google_out);
    fix_google_param(google_sig);
    fix_google_param(google_reqId);
    fix_google_param(google_version);
    fix_google_param(responseHandler);
    fix_google_param(outFileName);

     if(!chart || !*chart) {
         buffer_sprintf(w->response.data, "No chart id is given at the request.");
         goto cleanup;
    }

    RRDSET *st = rrdset_find(host, chart);
    if(!st) st = rrdset_find_byname(host, chart);
    if(!st) {
        buffer_strcat(w->response.data, "Chart is not found: ");
        buffer_strcat_htmlescape(w->response.data, chart);
        ret = 404;
        goto cleanup;
    }
    st->last_accessed_time = now_realtime_sec();

    long long before = (before_str && *before_str)?str2l(before_str):0;
    long long after  = (after_str  && *after_str) ?str2l(after_str):0;
    int       points = (points_str && *points_str)?str2i(points_str):0;
    long      group_time = (group_time_str && *group_time_str)?str2l(group_time_str):0;

    debug(D_WEB_CLIENT, "%llu: API command 'data' for chart '%s', dimensions '%s', after '%lld', before '%lld', points '%d', group '%d', format '%u', options '0x%08x'"
          , w->id
          , chart
          , (dimensions)?buffer_tostring(dimensions):""
          , after
          , before
          , points
          , group
          , format
          , options
    );

    if(outFileName && *outFileName) {
        buffer_sprintf(w->response.header, "Content-Disposition: attachment; filename=\"%s\"\r\n", outFileName);
        debug(D_WEB_CLIENT, "%llu: generating outfilename header: '%s'", w->id, outFileName);
    }

    if(format == DATASOURCE_DATATABLE_JSONP) {
        if(responseHandler == NULL)
            responseHandler = "google.visualization.Query.setResponse";

        debug(D_WEB_CLIENT_ACCESS, "%llu: GOOGLE JSON/JSONP: version = '%s', reqId = '%s', sig = '%s', out = '%s', responseHandler = '%s', outFileName = '%s'",
                w->id, google_version, google_reqId, google_sig, google_out, responseHandler, outFileName
        );

        buffer_sprintf(w->response.data,
                "%s({version:'%s',reqId:'%s',status:'ok',sig:'%ld',table:",
                responseHandler, google_version, google_reqId, st->last_updated.tv_sec);
    }
    else if(format == DATASOURCE_JSONP) {
        if(responseHandler == NULL)
            responseHandler = "callback";

        buffer_strcat(w->response.data, responseHandler);
        buffer_strcat(w->response.data, "(");
    }

    ret = rrdset2anything_api_v1(st, w->response.data, dimensions, format, points, after, before, group, group_time
                                 , options, &last_timestamp_in_data);

    if(format == DATASOURCE_DATATABLE_JSONP) {
        if(google_timestamp < last_timestamp_in_data)
            buffer_strcat(w->response.data, "});");

        else {
            buffer_flush(w->response.data);
            buffer_sprintf(w->response.data,
                    "%s({version:'%s',reqId:'%s',status:'error',errors:[{reason:'not_modified',message:'Data not modified'}]});",
                    responseHandler, google_version, google_reqId);
        }
    }
    else if(format == DATASOURCE_JSONP)
        buffer_strcat(w->response.data, ");");

    cleanup:
    buffer_free(dimensions);
    return ret;
}
