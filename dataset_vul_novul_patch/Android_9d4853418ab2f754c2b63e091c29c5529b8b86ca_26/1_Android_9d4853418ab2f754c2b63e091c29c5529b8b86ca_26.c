validate_T(void)
 /* Validate the above table - this just builds the above values */

 {
    unsigned int i;
 
   for (i=0; i<TTABLE_SIZE; ++i)
    {
       if (transform_info[i].when & TRANSFORM_R)
          read_transforms |= transform_info[i].transform;

 if (transform_info[i].when & TRANSFORM_W)
         write_transforms |= transform_info[i].transform;
 }

 /* Reversible transforms are those which are supported on both read and
    * write.
    */
   rw_transforms = read_transforms & write_transforms;
}
