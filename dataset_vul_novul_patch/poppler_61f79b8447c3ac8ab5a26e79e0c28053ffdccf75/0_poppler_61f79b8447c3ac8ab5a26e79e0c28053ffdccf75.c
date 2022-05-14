bool extractPages (const char *srcFileName, const char *destFileName) {
  char pathName[4096];
  GooString *gfileName = new GooString (srcFileName);
  PDFDoc *doc = new PDFDoc (gfileName, NULL, NULL, NULL);

  if (!doc->isOk()) {
    error(errSyntaxError, -1, "Could not extract page(s) from damaged file ('{0:s}')", srcFileName);
    return false;
  }

  if (firstPage == 0 && lastPage == 0) {
    firstPage = 1;
    lastPage = doc->getNumPages();
  }
  if (lastPage == 0)
    lastPage = doc->getNumPages();
  if (firstPage == 0)
   if (firstPage == 0)
     firstPage = 1;
   if (firstPage != lastPage && strstr(destFileName, "%d") == NULL) {
    error(errSyntaxError, -1, "'{0:s}' must contain '%d' if more than one page should be extracted", destFileName);
     return false;
   }
  
  // destFileName can have multiple %% and one %d
  // We use auxDestFileName to replace all the valid % appearances
  // by 'A' (random char that is not %), if at the end of replacing
  // any of the valid appearances there is still any % around, the
  // pattern is wrong
  char *auxDestFileName = strdup(destFileName);
  // %% can appear as many times as you want
  char *p = strstr(auxDestFileName, "%%");
  while (p != NULL) {
    *p = 'A';
    *(p + 1) = 'A';
    p = strstr(p, "%%"); 
  }
  // %d can appear only one time
  p = strstr(auxDestFileName, "%d");
  if (p != NULL) {
    *p = 'A';
  }
  // at this point any other % is wrong
  p = strstr(auxDestFileName, "%");
  if (p != NULL) {
    error(errSyntaxError, -1, "'{0:s}' can only contain one '%d' pattern", destFileName);
    free(auxDestFileName);
    return false;
  }
  free(auxDestFileName);
  
   for (int pageNo = firstPage; pageNo <= lastPage; pageNo++) {
     snprintf (pathName, sizeof (pathName) - 1, destFileName, pageNo);
     GooString *gpageName = new GooString (pathName);
	{
	  printUsage ("pdfseparate", "<PDF-sourcefile> <PDF-pattern-destfile>",
		      argDesc);
	}
      if (printVersion || printHelp)
	exitCode = 0;
      goto err0;
    }
  globalParams = new GlobalParams();
  ok = extractPages (argv[1], argv[2]);
  if (ok) {
    exitCode = 0;
  }
  delete globalParams;

err0:

  return exitCode;
}
