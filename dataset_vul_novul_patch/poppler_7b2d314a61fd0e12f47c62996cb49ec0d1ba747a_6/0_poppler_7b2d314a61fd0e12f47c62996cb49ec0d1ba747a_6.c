void GfxIndexedColorSpace::getRGBLine(Guchar *in, unsigned int *out, int length) {
  Guchar *line;
   int i, j, n;
 
   n = base->getNComps();
  line = (Guchar *) gmallocn (length, n);
   for (i = 0; i < length; i++)
     for (j = 0; j < n; j++)
       line[i * n + j] = lookup[in[i] * n + j];

  base->getRGBLine(line, out, length);

  gfree (line);
}
