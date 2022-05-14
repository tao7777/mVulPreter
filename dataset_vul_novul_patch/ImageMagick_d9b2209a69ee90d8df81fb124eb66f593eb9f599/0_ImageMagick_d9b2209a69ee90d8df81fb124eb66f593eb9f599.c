static void InsertRow(unsigned char *p,ssize_t y,Image *image, int bpp)
{
  ExceptionInfo
    *exception;

  int
    bit;

  ssize_t
    x;

  register PixelPacket
    *q;

  IndexPacket
    index;

  register IndexPacket
    *indexes;

  exception=(&image->exception);
  switch (bpp)
    {
    case 1:  /* Convert bitmap scanline. */
      {
        q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=GetAuthenticIndexQueue(image);
        for (x=0; x < ((ssize_t) image->columns-7); x+=8)
          {
            for (bit=0; bit < 8; bit++)
              {
                index=((*p) & (0x80 >> bit) ? 0x01 : 0x00);
                SetPixelIndex(indexes+x+bit,index);
                SetPixelRGBO(q,image->colormap+(ssize_t) index);
                q++;
              }
            p++;
          }
        if ((image->columns % 8) != 0)
          {
            for (bit=0; bit < (ssize_t) (image->columns % 8); bit++)
              {
                index=((*p) & (0x80 >> bit) ? 0x01 : 0x00);
                SetPixelIndex(indexes+x+bit,index);
                SetPixelRGBO(q,image->colormap+(ssize_t) index);
                q++;
              }
            p++;
          }
        if (!SyncAuthenticPixels(image,exception))
          break;
        break;
      }
    case 2:  /* Convert PseudoColor scanline. */
      {
        q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
         if (q == (PixelPacket *) NULL)
           break;
         indexes=GetAuthenticIndexQueue(image);
        for (x=0; x < ((ssize_t) image->columns-3); x+=4)
         {
             index=ConstrainColormapIndex(image,(*p >> 6) & 0x3);
             SetPixelIndex(indexes+x,index);
            SetPixelRGBO(q,image->colormap+(ssize_t) index);
            q++;
            index=ConstrainColormapIndex(image,(*p >> 4) & 0x3);
            SetPixelIndex(indexes+x,index);
            SetPixelRGBO(q,image->colormap+(ssize_t) index);
            q++;
            index=ConstrainColormapIndex(image,(*p >> 2) & 0x3);
            SetPixelIndex(indexes+x,index);
            SetPixelRGBO(q,image->colormap+(ssize_t) index);
            q++;
             index=ConstrainColormapIndex(image,(*p) & 0x3);
             SetPixelIndex(indexes+x+1,index);
             SetPixelRGBO(q,image->colormap+(ssize_t) index);
             q++;
            p++;
         }
        if ((image->columns % 4) != 0)
           {
             index=ConstrainColormapIndex(image,(*p >> 6) & 0x3);
             SetPixelIndex(indexes+x,index);
             SetPixelRGBO(q,image->colormap+(ssize_t) index);
             q++;
            if ((image->columns % 4) > 1)
               {
                 index=ConstrainColormapIndex(image,(*p >> 4) & 0x3);
                 SetPixelIndex(indexes+x,index);
                 SetPixelRGBO(q,image->colormap+(ssize_t) index);
                 q++;
                if ((image->columns % 4) > 2)
                   {
                     index=ConstrainColormapIndex(image,(*p >> 2) & 0x3);
                     SetPixelIndex(indexes+x,index);
                    SetPixelRGBO(q,image->colormap+(ssize_t) index);
                    q++;
                  }
              }
            p++;
          }
        if (SyncAuthenticPixels(image,exception) == MagickFalse)
          break;
        break;
      }

    case 4:  /* Convert PseudoColor scanline. */
      {
        q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=GetAuthenticIndexQueue(image);
        for (x=0; x < ((ssize_t) image->columns-1); x+=2)
          {
            index=ConstrainColormapIndex(image,(*p >> 4) & 0x0f);
            SetPixelIndex(indexes+x,index);
            SetPixelRGBO(q,image->colormap+(ssize_t) index);
            q++;
            index=ConstrainColormapIndex(image,(*p) & 0x0f);
            SetPixelIndex(indexes+x+1,index);
            SetPixelRGBO(q,image->colormap+(ssize_t) index);
            p++;
            q++;
          }
        if ((image->columns % 2) != 0)
          {
            index=ConstrainColormapIndex(image,(*p >> 4) & 0x0f);
            SetPixelIndex(indexes+x,index);
            SetPixelRGBO(q,image->colormap+(ssize_t) index);
            p++;
            q++;
          }
        if (SyncAuthenticPixels(image,exception) == MagickFalse)
          break;
        break;
      }
    case 8: /* Convert PseudoColor scanline. */
      {
        q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
        if (q == (PixelPacket *) NULL) break;
        indexes=GetAuthenticIndexQueue(image);

        for (x=0; x < (ssize_t) image->columns; x++)
          {
            index=ConstrainColormapIndex(image,*p);
            SetPixelIndex(indexes+x,index);
            SetPixelRGBO(q,image->colormap+(ssize_t) index);
            p++;
            q++;
          }
        if (SyncAuthenticPixels(image,exception) == MagickFalse)
          break;
      }
      break;

    case 24:     /*  Convert DirectColor scanline.  */
      q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
      if (q == (PixelPacket *) NULL)
        break;
      for (x=0; x < (ssize_t) image->columns; x++)
        {
          SetPixelRed(q,ScaleCharToQuantum(*p++));
          SetPixelGreen(q,ScaleCharToQuantum(*p++));
          SetPixelBlue(q,ScaleCharToQuantum(*p++));
          q++;
        }
      if (!SyncAuthenticPixels(image,exception))
        break;
      break;
    }
}
