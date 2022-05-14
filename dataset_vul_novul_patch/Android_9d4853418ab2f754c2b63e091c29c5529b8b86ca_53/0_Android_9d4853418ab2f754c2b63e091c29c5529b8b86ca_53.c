image_transform_add(PNG_CONST image_transform **this, unsigned int max,
image_transform_add(const image_transform **this, unsigned int max,
    png_uint_32 counter, char *name, size_t sizeof_name, size_t *pos,
    png_byte colour_type, png_byte bit_depth)
 {
 for (;;) /* until we manage to add something */
 {
      png_uint_32 mask;
      image_transform *list;

 /* Find the next counter value, if the counter is zero this is the start
       * of the list.  This routine always returns the current counter (not the
       * next) so it returns 0 at the end and expects 0 at the beginning.
       */
 if (counter == 0) /* first time */
 {
         image_transform_reset_count();
 if (max <= 1)
            counter = 1;
 else
            counter = random_32();
 }
 else /* advance the counter */
 {
 switch (max)
 {
 case 0: ++counter; break;
 case 1:  counter <<= 1; break;
 default: counter = random_32(); break;
 }
 }

 /* Now add all these items, if possible */
 *this = &image_transform_end;
 list = image_transform_first;
      mask = 1;

 /* Go through the whole list adding anything that the counter selects: */
 while (list != &image_transform_end)
 {
 if ((counter & mask) != 0 && list->enable &&
 (max == 0 || list->local_use < max))
 {
 /* Candidate to add: */
 if (list->add(list, this, colour_type, bit_depth) || max == 0)
 {
 /* Added, so add to the name too. */
 *pos = safecat(name, sizeof_name, *pos, " +");
 *pos = safecat(name, sizeof_name, *pos, list->name);
 }

 else
 {
 /* Not useful and max>0, so remove it from *this: */
 *this = list->next;
 list->next = 0;

 /* And, since we know it isn't useful, stop it being added again
                * in this run:
                */
 list->local_use = max;
 }
 }

         mask <<= 1;
 list = list->list;
 }

 /* Now if anything was added we have something to do. */
 if (*this != &image_transform_end)
 return counter;

 /* Nothing added, but was there anything in there to add? */
 if (!image_transform_test_counter(counter, max))
 return 0;

    }
 }
