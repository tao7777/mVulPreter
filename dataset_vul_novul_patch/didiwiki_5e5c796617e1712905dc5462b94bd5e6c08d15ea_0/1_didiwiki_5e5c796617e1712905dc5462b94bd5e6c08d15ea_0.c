 wiki_handle_rest_call(HttpRequest  *req, 
 		      HttpResponse *res,
		      char         *func)
{

  if (func != NULL && *func != '\0')
    {
      if (!strcmp(func, "page/get"))
	{
	  char *page = http_request_param_get(req, "page");

 	  if (page == NULL)
 	    page = http_request_get_query_string(req);
 
	  if (page && (access(page, R_OK) == 0)) 
 	    {
 	      http_response_printf(res, "%s", file_read(page));
 	      http_response_send(res);
	      return;
	    }  
	}
      else if (!strcmp(func, "page/set"))
	{
	  char *wikitext = NULL, *page = NULL;
 	  if( ( (wikitext = http_request_param_get(req, "text")) != NULL)
 	      && ( (page = http_request_param_get(req, "page")) != NULL))
 	    {
	      file_write(page, wikitext);	      
 	      http_response_printf(res, "success");
 	      http_response_send(res);
 	      return;
 	    }
 	}
       else if (!strcmp(func, "page/delete"))
 	{
	  char *page = http_request_param_get(req, "page");

 	  if (page == NULL)
 	    page = http_request_get_query_string(req);
 
	  if (page && (unlink(page) > 0))
 	    {
 	      http_response_printf(res, "success");
 	      http_response_send(res);
	      return;  
	    }
	}
      else if (!strcmp(func, "page/exists"))
	{
	  char *page = http_request_param_get(req, "page");

 	  if (page == NULL)
 	    page = http_request_get_query_string(req);
 
	  if (page && (access(page, R_OK) == 0)) 
 	    {
 	      http_response_printf(res, "success");
 	      http_response_send(res);
	      return;  
	    }
	}
      else if (!strcmp(func, "pages") || !strcmp(func, "search"))
	{
	  WikiPageList **pages = NULL;
	  int            n_pages, i;
	  char          *expr = http_request_param_get(req, "expr");

	  if (expr == NULL)
	    expr = http_request_get_query_string(req);
	  
	  pages = wiki_get_pages(&n_pages, expr);

	  if (pages)
	    {
	      for (i=0; i<n_pages; i++)
		{
		  struct tm   *pTm;
		  char   datebuf[64];
		  
		  pTm = localtime(&pages[i]->mtime);
		  strftime(datebuf, sizeof(datebuf), "%Y-%m-%d %H:%M", pTm);
		  http_response_printf(res, "%s\t%s\n", pages[i]->name, datebuf);
		}

	      http_response_send(res);
	      return;  
	    }
	}
    }

  http_response_set_status(res, 500, "Error");
  http_response_printf(res, "<html><body>Failed</body></html>\n");
  http_response_send(res);

  return;  
}
