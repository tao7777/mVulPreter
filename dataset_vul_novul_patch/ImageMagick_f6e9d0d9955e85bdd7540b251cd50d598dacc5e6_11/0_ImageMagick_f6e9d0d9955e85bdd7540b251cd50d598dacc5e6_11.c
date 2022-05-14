static Image *ReadOneDJVUImage(LoadContext* lc,const int pagenum,
  const ImageInfo *image_info,ExceptionInfo *exception)
{
  ddjvu_page_type_t
     type;

  ddjvu_pageinfo_t info;
  ddjvu_message_t *message;
   Image *image;
   int logging;
   int tag;
  MagickBooleanType status;
 
         /* so, we know that the page is there! Get its dimension, and  */
 
        /* Read one DJVU image */
        image = lc->image;

        /* register PixelPacket *q; */

        logging=LogMagickEvent(CoderEvent,GetMagickModule(), "  enter ReadOneDJVUImage()");
        (void) logging;

#if DEBUG
        printf("====  Loading the page %d\n", pagenum);
#endif
        lc->page = ddjvu_page_create_by_pageno(lc->document, pagenum); /*  0? */

        /* pump data untill the page is ready for rendering. */
        tag=(-1);
        do {
                while ((message = ddjvu_message_peek(lc->context)))
                        {
                                tag=process_message(message);
                                if (tag == 0) break;
                                ddjvu_message_pop(lc->context);
                        }
                /* fixme: maybe exit? */
                /* if (lc->error) break; */

                message = pump_data_until_message(lc,image);
                if (message)
                        do {
                                tag=process_message(message);
                                if (tag == 0) break;
                                ddjvu_message_pop(lc->context);
                        } while ((message = ddjvu_message_peek(lc->context)));
        } while (!ddjvu_page_decoding_done(lc->page));

        ddjvu_document_get_pageinfo(lc->document, pagenum, &info);

        image->x_resolution = (float) info.dpi;
        image->y_resolution =(float) info.dpi;
        if (image_info->density != (char *) NULL)
          {
            int
              flags;

            GeometryInfo
              geometry_info;

            /*
              Set rendering resolution.
            */
            flags=ParseGeometry(image_info->density,&geometry_info);
            image->x_resolution=geometry_info.rho;
            image->y_resolution=geometry_info.sigma;
            if ((flags & SigmaValue) == 0)
              image->y_resolution=image->x_resolution;
            info.width=(int) (info.width*image->x_resolution/info.dpi);
            info.height=(int) (info.height*image->y_resolution/info.dpi);
            info.dpi=(int) MagickMax(image->x_resolution,image->y_resolution);
          }
        type = ddjvu_page_get_type(lc->page);

        /* double -> float! */
        /* image->gamma = (float)ddjvu_page_get_gamma(lc->page); */

        /* mmc:  set  image->depth  */
        /* mmc:  This from the type */

        image->columns=(size_t) info.width;
        image->rows=(size_t) info.height;

        /* mmc: bitonal should be palettized, and compressed! */
        if (type == DDJVU_PAGETYPE_BITONAL){
                image->colorspace = GRAYColorspace;
                image->storage_class = PseudoClass;
                image->depth =  8UL;    /* i only support that? */
                image->colors= 2;
                if (AcquireImageColormap(image,image->colors) == MagickFalse)
                  ThrowReaderException(ResourceLimitError,
                   "MemoryAllocationFailed");
        } else {
                image->colorspace = RGBColorspace;
                image->storage_class = DirectClass;
                /* fixme:  MAGICKCORE_QUANTUM_DEPTH ?*/
                image->depth =  8UL;    /* i only support that? */

                 image->matte = MagickTrue;
                 /* is this useful? */
         }
        status=SetImageExtent(image,image->columns,image->rows);
        if (status == MagickFalse)
          {
            InheritException(exception,&image->exception);
            return(DestroyImageList(image));
          }
 #if DEBUG
         printf("now filling %.20g x %.20g\n",(double) image->columns,(double)
           image->rows);
#endif


#if 1                           /* per_line */

        /* q = QueueAuthenticPixels(image,0,0,image->columns,image->rows); */
        get_page_image(lc, lc->page, 0, 0, info.width, info.height, image_info);
#else
        int i;
        for (i = 0;i< image->rows; i++)
                {
                        printf("%d\n",i);
                        q = QueueAuthenticPixels(image,0,i,image->columns,1);
                        get_page_line(lc, i, quantum_info);
                        SyncAuthenticPixels(image);
                }

#endif /* per_line */


#if DEBUG
        printf("END: finished filling %.20g x %.20g\n",(double) image->columns,
          (double) image->rows);
#endif

        if (!image->ping)
          SyncImage(image);
        /* indexes=GetAuthenticIndexQueue(image); */
        /* mmc: ??? Convert PNM pixels to runlength-encoded MIFF packets. */
        /* image->colors =  */

        /* how is the line padding  / stride? */

        if (lc->page) {
                ddjvu_page_release(lc->page);
                lc->page = NULL;
        }

        /* image->page.y=mng_info->y_off[mng_info->object_id]; */
        if (tag == 0)
          image=DestroyImage(image);
        return image;
        /* end of reading one DJVU page/image */
}
