transform_disable(PNG_CONST char *name)
 {
    image_transform *list = image_transform_first;
 
 while (list != &image_transform_end)
 {
 if (strcmp(list->name, name) == 0)
 {
 list->enable = 0;
 return;
 }

 list = list->list;
 }

   fprintf(stderr, "pngvalid: --transform-disable=%s: unknown transform\n",
      name);
   exit(99);
}
