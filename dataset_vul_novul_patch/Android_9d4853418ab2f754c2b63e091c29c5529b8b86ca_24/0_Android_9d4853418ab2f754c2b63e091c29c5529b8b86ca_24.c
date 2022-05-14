transform_name(int t)
 /* The name, if 't' has multiple bits set the name of the lowest set bit is
    * returned.
    */
{
 unsigned int i;

 
    t &= -t; /* first set bit */
 
   for (i=0; i<TTABLE_SIZE; ++i) if (transform_info[i].name != NULL)
    {
       if ((transform_info[i].transform & t) != 0)
          return transform_info[i].name;
 }

 return "invalid transform";
}
