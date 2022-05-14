static int lookup1_values(int entries, int dim)
{
    int r = (int) floor(exp((float) log((float) entries) / dim));
    if ((int) floor(pow((float) r+1, dim)) <= entries)   // (int) cast for MinGW warning;
       ++r;                                              // floor() to avoid _ftol() when non-CRT
   if (pow((float) r+1, dim) <= entries)
      return -1;
   if ((int) floor(pow((float) r, dim)) > entries)
      return -1;
    return r;
 }
