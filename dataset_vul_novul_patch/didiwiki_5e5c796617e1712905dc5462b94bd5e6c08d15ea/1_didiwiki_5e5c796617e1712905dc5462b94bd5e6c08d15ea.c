wiki_handle_http_request(HttpRequest *req)
{
  HttpResponse *res      = http_response_new(req);
  char         *page     = http_request_get_path_info(req); 
  char         *command  = http_request_get_query_string(req); 
  char         *wikitext = "";

  util_dehttpize(page); 	/* remove any encoding on the requested
				   page name.                           */

  if (!strcmp(page, "/"))
    {
      if (access("WikiHome", R_OK) != 0)
	wiki_redirect(res, "/WikiHome?create");
      page = "/WikiHome";
    }

  if (!strcmp(page, "/styles.css"))
    {
      /*  Return CSS page */
      http_response_set_content_type(res, "text/css");
      http_response_printf(res, "%s", CssData);
      http_response_send(res);
      exit(0);
    }

  if (!strcmp(page, "/favicon.ico"))
    {
      /*  Return favicon */
      http_response_set_content_type(res, "image/ico");
      http_response_set_data(res, FaviconData, FaviconDataLen);
      http_response_send(res);
      exit(0);
    }


  page = page + 1; 		/* skip slash */

  if (!strncmp(page, "api/", 4))
    {
      char *p;

      page += 4; 
      for (p=page; *p != '\0'; p++)
	if (*p=='?') { *p ='\0'; break; }
      
      wiki_handle_rest_call(req, res, page); 
      exit(0);
    }

   /* A little safety. issue a malformed request for any paths,
    * There shouldn't need to be any..
    */
  if (strchr(page, '/'))
     {
       http_response_set_status(res, 404, "Not Found");
       http_response_printf(res, "<html><body>404 Not Found</body></html>\n");
      http_response_send(res);
      exit(0);
    }

  if (!strcmp(page, "Changes"))
    {
      wiki_show_changes_page(res);
    }
  else if (!strcmp(page, "ChangesRss"))
    {
      wiki_show_changes_page_rss(res);
    }
  else if (!strcmp(page, "Search"))
    {
      wiki_show_search_results_page(res, http_request_param_get(req, "expr"));
    }
  else if (!strcmp(page, "Create"))
    {
      if ( (wikitext = http_request_param_get(req, "title")) != NULL)
	{
	  /* create page and redirect */
	  wiki_redirect(res, http_request_param_get(req, "title"));
	}
      else
	{
	   /* show create page form  */
	  wiki_show_create_page(res);
	}
    }
  else
    {
      /* TODO: dont blindly write wikitext data to disk */
      if ( (wikitext = http_request_param_get(req, "wikitext")) != NULL)
	{
	  file_write(page, wikitext);	      
	}

      if (access(page, R_OK) == 0) 	/* page exists */
	{
	  wikitext = file_read(page);
	  
	  if (!strcmp(command, "edit"))
	    {
	      /* print edit page */
	      wiki_show_edit_page(res, wikitext, page);
	    }
	  else
	    {
	      wiki_show_page(res, wikitext, page);
	    }
	}
      else
	{
	  if (!strcmp(command, "create"))
	    {
	      wiki_show_edit_page(res, NULL, page);
	    }
	  else
	    {
	      char buf[1024];
	      snprintf(buf, 1024, "%s?create", page);
	      wiki_redirect(res, buf);
	    }
	}
    }

}
