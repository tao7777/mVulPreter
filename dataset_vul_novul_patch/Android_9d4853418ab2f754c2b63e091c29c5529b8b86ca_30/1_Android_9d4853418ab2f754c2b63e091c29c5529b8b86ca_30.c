read_one_file(Image *image)
{
 if (!(image->opts & READ_FILE) || (image->opts & USE_STDIO))
 {
 /* memory or stdio. */
 FILE *f = fopen(image->file_name, "rb");

 if (f != NULL)
 {
 if (image->opts & READ_FILE)
            image->input_file = f;

 else /* memory */
 {
 if (fseek(f, 0, SEEK_END) == 0)

             {
                long int cb = ftell(f);
 
               if (cb > 0 && (unsigned long int)cb < (size_t)~(size_t)0)
                {
                  png_bytep b = voidcast(png_bytep, malloc((size_t)cb));
                  if (b != NULL)
                   {
                     rewind(f);
 
                     if (fread(b, (size_t)cb, 1, f) == 1)
                      {
                        fclose(f);
                        image->input_memory_size = cb;
                        image->input_memory = b;
                      }
 
                      else
                     {
                        free(b);
                         return logclose(image, f, image->file_name,
                           ": read failed: ");
                     }
                   }
 
                   else
                      return logclose(image, f, image->file_name,
                        ": out of memory: ");
                }
 
                else if (cb == 0)
 return logclose(image, f, image->file_name,
 ": zero length: ");

 else
 return logclose(image, f, image->file_name,
 ": tell failed: ");
 }

 else
 return logclose(image, f, image->file_name, ": seek failed: ");
 }
 }

 else
 return logerror(image, image->file_name, ": open failed: ",
            strerror(errno));
 }

 return read_file(image, FORMAT_NO_CHANGE, NULL);
}
