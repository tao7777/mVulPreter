static int lookup1_values(int entries, int dim)
{
    int r = (int) floor(exp((float) log((float) entries) / dim));
    if ((int) floor(pow((float) r+1, dim)) <= entries)   // (int) cast for MinGW warning;
       ++r;                                              // floor() to avoid _ftol() when non-CRT
   assert(pow((float) r+1, dim) > entries);
   assert((int) floor(pow((float) r, dim)) <= entries); // (int),floor() as above
    return r;
 }
