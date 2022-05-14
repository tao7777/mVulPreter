png_set_filter_heuristics(png_structp png_ptr, int heuristic_method,
    int num_weights, png_doublep filter_weights,
    png_doublep filter_costs)
 {
   int i;
   png_debug(1, "in png_set_filter_heuristics");
   if (png_ptr == NULL)
      return;
   if (heuristic_method >= PNG_FILTER_HEURISTIC_LAST)
   {
      png_warning(png_ptr, "Unknown filter heuristic method");
      return;
   }
   if (heuristic_method == PNG_FILTER_HEURISTIC_DEFAULT)
   {
      heuristic_method = PNG_FILTER_HEURISTIC_UNWEIGHTED;
   }
   if (num_weights < 0 || filter_weights == NULL ||
      heuristic_method == PNG_FILTER_HEURISTIC_UNWEIGHTED)
   {
      num_weights = 0;
   }
   png_ptr->num_prev_filters = (png_byte)num_weights;
   png_ptr->heuristic_method = (png_byte)heuristic_method;
   if (num_weights > 0)
   {
      if (png_ptr->prev_filters == NULL)
      {
         png_ptr->prev_filters = (png_bytep)png_malloc(png_ptr,
            (png_uint_32)(png_sizeof(png_byte) * num_weights));
         /* To make sure that the weighting starts out fairly */
         for (i = 0; i < num_weights; i++)
         {
            png_ptr->prev_filters[i] = 255;
         }
      }
      if (png_ptr->filter_weights == NULL)
      {
         png_ptr->filter_weights = (png_uint_16p)png_malloc(png_ptr,
            (png_uint_32)(png_sizeof(png_uint_16) * num_weights));
         png_ptr->inv_filter_weights = (png_uint_16p)png_malloc(png_ptr,
            (png_uint_32)(png_sizeof(png_uint_16) * num_weights));
         for (i = 0; i < num_weights; i++)
         {
            png_ptr->inv_filter_weights[i] =
            png_ptr->filter_weights[i] = PNG_WEIGHT_FACTOR;
         }
      }
      for (i = 0; i < num_weights; i++)
      {
         if (filter_weights[i] < 0.0)
         {
            png_ptr->inv_filter_weights[i] =
            png_ptr->filter_weights[i] = PNG_WEIGHT_FACTOR;
         }
         else
         {
            png_ptr->inv_filter_weights[i] =
               (png_uint_16)((double)PNG_WEIGHT_FACTOR*filter_weights[i]+0.5);
            png_ptr->filter_weights[i] =
               (png_uint_16)((double)PNG_WEIGHT_FACTOR/filter_weights[i]+0.5);
         }
      }
   }
   /* If, in the future, there are other filter methods, this would
    * need to be based on png_ptr->filter.
    */
   if (png_ptr->filter_costs == NULL)
   {
      png_ptr->filter_costs = (png_uint_16p)png_malloc(png_ptr,
         (png_uint_32)(png_sizeof(png_uint_16) * PNG_FILTER_VALUE_LAST));
      png_ptr->inv_filter_costs = (png_uint_16p)png_malloc(png_ptr,
         (png_uint_32)(png_sizeof(png_uint_16) * PNG_FILTER_VALUE_LAST));
      for (i = 0; i < PNG_FILTER_VALUE_LAST; i++)
      {
         png_ptr->inv_filter_costs[i] =
         png_ptr->filter_costs[i] = PNG_COST_FACTOR;
      }
   }
   /* Here is where we set the relative costs of the different filters.  We
    * should take the desired compression level into account when setting
    * the costs, so that Paeth, for instance, has a high relative cost at low
    * compression levels, while it has a lower relative cost at higher
    * compression settings.  The filter types are in order of increasing
    * relative cost, so it would be possible to do this with an algorithm.
    */
   for (i = 0; i < PNG_FILTER_VALUE_LAST; i++)
   {
      if (filter_costs == NULL || filter_costs[i] < 0.0)
      {
         png_ptr->inv_filter_costs[i] =
         png_ptr->filter_costs[i] = PNG_COST_FACTOR;
      }
      else if (filter_costs[i] >= 1.0)
      {
         png_ptr->inv_filter_costs[i] =
            (png_uint_16)((double)PNG_COST_FACTOR / filter_costs[i] + 0.5);
         png_ptr->filter_costs[i] =
            (png_uint_16)((double)PNG_COST_FACTOR * filter_costs[i] + 0.5);
      }
   }
 }
