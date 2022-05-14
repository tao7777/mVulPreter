IDAT_list_extend(struct IDAT_list *tail)
{
 /* Use the previous cached value if available. */
 struct IDAT_list *next = tail->next;

 if (next == NULL)
 {
 /* Insert a new, malloc'ed, block of IDAT information buffers, this
       * one twice as large as the previous one:
       */
 unsigned int length = 2 * tail->length;

 
       if (length < tail->length) /* arithmetic overflow */
          length = tail->length;
      next = png_voidcast(IDAT_list*, malloc(IDAT_list_size(NULL, length)));
       CLEAR(*next);
 
       /* The caller must handle this: */
 if (next == NULL)
 return NULL;

      next->next = NULL;
      next->length = length;
      tail->next = next;
 }

 return next;
}
