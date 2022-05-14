pdf_t *pdf_new(const char *name)
{
     const char *n;
     pdf_t      *pdf;
    
    pdf = calloc(1, sizeof(pdf_t));
 
     if (name)
     {
        /* Just get the file name (not path) */
        if ((n = strrchr(name, '/')))
          ++n;
         else
           n = name;
 
        pdf->name = malloc(strlen(n) + 1);
         strcpy(pdf->name, n);
     }
     else /* !name */
     {
        pdf->name = malloc(strlen("Unknown") + 1);
         strcpy(pdf->name, "Unknown");
     }
 
    return pdf;
}
