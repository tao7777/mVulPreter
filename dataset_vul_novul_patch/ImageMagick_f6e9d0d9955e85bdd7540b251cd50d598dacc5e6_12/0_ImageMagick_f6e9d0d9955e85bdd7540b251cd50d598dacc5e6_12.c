get_page_image(LoadContext *lc, ddjvu_page_t *page, int x, int y, int w, int h, const ImageInfo *image_info ) {
  ddjvu_format_t
    *format;

  ddjvu_page_type_t
    type;

  Image
    *image;

  int
    ret,
    stride;

  unsigned char
    *q;

        ddjvu_rect_t rect;
        rect.x = x;
        rect.y = y;
        rect.w = (unsigned int) w;             /* /10 */
        rect.h = (unsigned int) h;             /* /10 */

        image = lc->image;
        type = ddjvu_page_get_type(lc->page);

        /* stride of this temporary buffer: */
        stride = (type == DDJVU_PAGETYPE_BITONAL)?
                (image->columns + 7)/8 : image->columns *3;

        q = (unsigned char *) AcquireQuantumMemory(image->rows,stride);
        if (q == (unsigned char *) NULL)
          return;

        format = ddjvu_format_create(
                (type == DDJVU_PAGETYPE_BITONAL)?DDJVU_FORMAT_LSBTOMSB : DDJVU_FORMAT_RGB24,
                /* DDJVU_FORMAT_RGB24
                 * DDJVU_FORMAT_RGBMASK32*/
                /* DDJVU_FORMAT_RGBMASK32 */
                0, NULL);

#if 0
        /* fixme:  ThrowReaderException is a macro, which uses  `exception' variable */
        if (format == NULL)
                {
                        abort();
                        /* ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed"); */
                }

#endif
        ddjvu_format_set_row_order(format, 1);
        ddjvu_format_set_y_direction(format, 1);

        ret = ddjvu_page_render(page,
                                    DDJVU_RENDER_COLOR, /* ddjvu_render_mode_t */
                                    &rect,
                                    &rect,     /* mmc: ?? */
                                    format,
                                    stride, /* ?? */
                                    (char*)q);
        (void) ret;
        ddjvu_format_release(format);


        if (type == DDJVU_PAGETYPE_BITONAL) {
                /*  */
#if DEBUG
                printf("%s: expanding BITONAL page/image\n", __FUNCTION__);
#endif
                register IndexPacket *indexes;
                size_t bit, byte;

                for (y=0; y < (ssize_t) image->rows; y++)
                        {
                                PixelPacket * o = QueueAuthenticPixels(image,0,y,image->columns,1,&image->exception);
                                if (o == (PixelPacket *) NULL)
                                        break;
                                indexes=GetAuthenticIndexQueue(image);
                                bit=0;
                                byte=0;

                                /* fixme:  the non-aligned, last =<7 bits ! that's ok!!!*/
                                for (x= 0; x < (ssize_t) image->columns; x++)
                                        {
                                                if (bit == 0) byte= (size_t) q[(y * stride) + (x / 8)];

                                                if (indexes != (IndexPacket *) NULL)
                                                  SetPixelIndex(indexes+x,(IndexPacket) (((byte & 0x01) != 0) ? 0x00 : 0x01));
                                                bit++;
                                                if (bit == 8)
                                                        bit=0;
                                                byte>>=1;
                                        }
                                 if (SyncAuthenticPixels(image,&image->exception) == MagickFalse)
                                         break;
                         }
                if (image->ping == MagickFalse)
                   SyncImage(image);
         } else {
 #if DEBUG
                printf("%s: expanding PHOTO page/image\n", __FUNCTION__);
#endif
                /* now transfer line-wise: */
                ssize_t i;
#if 0
                /* old: */
                char* r;
#else
                register PixelPacket *r;
                unsigned char *s;
#endif
                s=q;
                for (i = 0;i< (ssize_t) image->rows; i++)
                        {
#if DEBUG
                               if (i % 1000 == 0) printf("%d\n",i);
#endif
                               r = QueueAuthenticPixels(image,0,i,image->columns,1,&image->exception);
                               if (r == (PixelPacket *) NULL)
                                 break;
                  for (x=0; x < (ssize_t) image->columns; x++)
                  {
                    SetPixelRed(r,ScaleCharToQuantum(*s++));
                    SetPixelGreen(r,ScaleCharToQuantum(*s++));
                    SetPixelBlue(r,ScaleCharToQuantum(*s++));
                    r++;
                  }

                              (void) SyncAuthenticPixels(image,&image->exception);
                        }
        }
        q=(unsigned char *) RelinquishMagickMemory(q);
}
