static void ImportCbYCrYQuantum(const Image *image,QuantumInfo *quantum_info,
  const MagickSizeType number_pixels,const unsigned char *magick_restrict p,
  Quantum *magick_restrict q,ExceptionInfo *exception)
{
  QuantumAny
    range;

  register ssize_t
    x;

  unsigned int
    pixel;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  switch (quantum_info->depth)
  {
    case 10:
    {
      Quantum
        cbcr[4];

      pixel=0;
      if (quantum_info->pack == MagickFalse)
        {
          register ssize_t
            i;

          size_t
            quantum;

          ssize_t
            n;
 
           n=0;
           quantum=0;
          for (x=0; x < (ssize_t) number_pixels; x+=4)
           {
             for (i=0; i < 4; i++)
             {
              switch (n % 3)
              {
                case 0:
                {
                  p=PushLongPixel(quantum_info->endian,p,&pixel);
                  quantum=(size_t) (ScaleShortToQuantum((unsigned short)
                    (((pixel >> 22) & 0x3ff) << 6)));
                  break;
                }
                case 1:
                {
                  quantum=(size_t) (ScaleShortToQuantum((unsigned short)
                    (((pixel >> 12) & 0x3ff) << 6)));
                  break;
                }
                case 2:
                {
                  quantum=(size_t) (ScaleShortToQuantum((unsigned short)
                    (((pixel >> 2) & 0x3ff) << 6)));
                  break;
                }
              }
              cbcr[i]=(Quantum) (quantum);
              n++;
            }
            p+=quantum_info->pad;
            SetPixelRed(image,cbcr[1],q);
            SetPixelGreen(image,cbcr[0],q);
            SetPixelBlue(image,cbcr[2],q);
            q+=GetPixelChannels(image);
            SetPixelRed(image,cbcr[3],q);
            SetPixelGreen(image,cbcr[0],q);
            SetPixelBlue(image,cbcr[2],q);
            q+=GetPixelChannels(image);
          }
          break;
        }
    }
    default:
    {
      range=GetQuantumRange(quantum_info->depth);
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        p=PushQuantumPixel(quantum_info,p,&pixel);
        SetPixelRed(image,ScaleAnyToQuantum(pixel,range),q);
        p=PushQuantumPixel(quantum_info,p,&pixel);
        SetPixelGreen(image,ScaleAnyToQuantum(pixel,range),q);
        q+=GetPixelChannels(image);
      }
      break;
    }
  }
}
