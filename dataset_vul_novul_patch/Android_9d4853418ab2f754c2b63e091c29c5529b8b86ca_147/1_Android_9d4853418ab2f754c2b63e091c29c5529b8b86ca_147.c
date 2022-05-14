transform_enable(PNG_CONST char *name)
 {
    /* Everything starts out enabled, so if we see an 'enable' disabled
     * everything else the first time round.
    */
 static int all_disabled = 0;
 int found_it = 0;
   image_transform *list = image_transform_first;

 while (list != &image_transform_end)
 {
 if (strcmp(list->name, name) == 0)
 {
 list->enable = 1;
         found_it = 1;
 }
 else if (!all_disabled)
 list->enable = 0;

 list = list->list;
 }

   all_disabled = 1;

 if (!found_it)
 {
      fprintf(stderr, "pngvalid: --transform-enable=%s: unknown transform\n",
         name);
      exit(99);
 }

 }
