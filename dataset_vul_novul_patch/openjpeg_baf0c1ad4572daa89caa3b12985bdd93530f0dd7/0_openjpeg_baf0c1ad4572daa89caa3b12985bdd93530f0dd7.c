static OPJ_BOOL bmp_read_info_header(FILE* IN, OPJ_BITMAPINFOHEADER* header)
{
    memset(header, 0, sizeof(*header));
    /* INFO HEADER */
    /* ------------- */
    header->biSize  = (OPJ_UINT32)getc(IN);
    header->biSize |= (OPJ_UINT32)getc(IN) << 8;
    header->biSize |= (OPJ_UINT32)getc(IN) << 16;
    header->biSize |= (OPJ_UINT32)getc(IN) << 24;

    switch (header->biSize) {
    case 12U:  /* BITMAPCOREHEADER */
    case 40U:  /* BITMAPINFOHEADER */
    case 52U:  /* BITMAPV2INFOHEADER */
    case 56U:  /* BITMAPV3INFOHEADER */
    case 108U: /* BITMAPV4HEADER */
    case 124U: /* BITMAPV5HEADER */
        break;
    default:
        fprintf(stderr, "Error, unknown BMP header size %d\n", header->biSize);
        return OPJ_FALSE;
    }

    header->biWidth  = (OPJ_UINT32)getc(IN);
    header->biWidth |= (OPJ_UINT32)getc(IN) << 8;
    header->biWidth |= (OPJ_UINT32)getc(IN) << 16;
    header->biWidth |= (OPJ_UINT32)getc(IN) << 24;

    header->biHeight  = (OPJ_UINT32)getc(IN);
    header->biHeight |= (OPJ_UINT32)getc(IN) << 8;
    header->biHeight |= (OPJ_UINT32)getc(IN) << 16;
    header->biHeight |= (OPJ_UINT32)getc(IN) << 24;

    header->biPlanes  = (OPJ_UINT16)getc(IN);
    header->biPlanes |= (OPJ_UINT16)((OPJ_UINT32)getc(IN) << 8);
 
     header->biBitCount  = (OPJ_UINT16)getc(IN);
     header->biBitCount |= (OPJ_UINT16)((OPJ_UINT32)getc(IN) << 8);
    if (header->biBitCount == 0) {
        fprintf(stderr, "Error, invalid biBitCount %d\n", 0);
        return OPJ_FALSE;
    }
 
     if (header->biSize >= 40U) {
         header->biCompression  = (OPJ_UINT32)getc(IN);
        header->biCompression |= (OPJ_UINT32)getc(IN) << 8;
        header->biCompression |= (OPJ_UINT32)getc(IN) << 16;
        header->biCompression |= (OPJ_UINT32)getc(IN) << 24;

        header->biSizeImage  = (OPJ_UINT32)getc(IN);
        header->biSizeImage |= (OPJ_UINT32)getc(IN) << 8;
        header->biSizeImage |= (OPJ_UINT32)getc(IN) << 16;
        header->biSizeImage |= (OPJ_UINT32)getc(IN) << 24;

        header->biXpelsPerMeter  = (OPJ_UINT32)getc(IN);
        header->biXpelsPerMeter |= (OPJ_UINT32)getc(IN) << 8;
        header->biXpelsPerMeter |= (OPJ_UINT32)getc(IN) << 16;
        header->biXpelsPerMeter |= (OPJ_UINT32)getc(IN) << 24;

        header->biYpelsPerMeter  = (OPJ_UINT32)getc(IN);
        header->biYpelsPerMeter |= (OPJ_UINT32)getc(IN) << 8;
        header->biYpelsPerMeter |= (OPJ_UINT32)getc(IN) << 16;
        header->biYpelsPerMeter |= (OPJ_UINT32)getc(IN) << 24;

        header->biClrUsed  = (OPJ_UINT32)getc(IN);
        header->biClrUsed |= (OPJ_UINT32)getc(IN) << 8;
        header->biClrUsed |= (OPJ_UINT32)getc(IN) << 16;
        header->biClrUsed |= (OPJ_UINT32)getc(IN) << 24;

        header->biClrImportant  = (OPJ_UINT32)getc(IN);
        header->biClrImportant |= (OPJ_UINT32)getc(IN) << 8;
        header->biClrImportant |= (OPJ_UINT32)getc(IN) << 16;
        header->biClrImportant |= (OPJ_UINT32)getc(IN) << 24;
    }

    if (header->biSize >= 56U) {
        header->biRedMask  = (OPJ_UINT32)getc(IN);
        header->biRedMask |= (OPJ_UINT32)getc(IN) << 8;
        header->biRedMask |= (OPJ_UINT32)getc(IN) << 16;
        header->biRedMask |= (OPJ_UINT32)getc(IN) << 24;

        header->biGreenMask  = (OPJ_UINT32)getc(IN);
        header->biGreenMask |= (OPJ_UINT32)getc(IN) << 8;
        header->biGreenMask |= (OPJ_UINT32)getc(IN) << 16;
        header->biGreenMask |= (OPJ_UINT32)getc(IN) << 24;

        header->biBlueMask  = (OPJ_UINT32)getc(IN);
        header->biBlueMask |= (OPJ_UINT32)getc(IN) << 8;
        header->biBlueMask |= (OPJ_UINT32)getc(IN) << 16;
        header->biBlueMask |= (OPJ_UINT32)getc(IN) << 24;

        header->biAlphaMask  = (OPJ_UINT32)getc(IN);
        header->biAlphaMask |= (OPJ_UINT32)getc(IN) << 8;
        header->biAlphaMask |= (OPJ_UINT32)getc(IN) << 16;
        header->biAlphaMask |= (OPJ_UINT32)getc(IN) << 24;
    }

    if (header->biSize >= 108U) {
        header->biColorSpaceType  = (OPJ_UINT32)getc(IN);
        header->biColorSpaceType |= (OPJ_UINT32)getc(IN) << 8;
        header->biColorSpaceType |= (OPJ_UINT32)getc(IN) << 16;
        header->biColorSpaceType |= (OPJ_UINT32)getc(IN) << 24;

        if (fread(&(header->biColorSpaceEP), 1U, sizeof(header->biColorSpaceEP),
                  IN) != sizeof(header->biColorSpaceEP)) {
            fprintf(stderr, "Error, can't  read BMP header\n");
            return OPJ_FALSE;
        }

        header->biRedGamma  = (OPJ_UINT32)getc(IN);
        header->biRedGamma |= (OPJ_UINT32)getc(IN) << 8;
        header->biRedGamma |= (OPJ_UINT32)getc(IN) << 16;
        header->biRedGamma |= (OPJ_UINT32)getc(IN) << 24;

        header->biGreenGamma  = (OPJ_UINT32)getc(IN);
        header->biGreenGamma |= (OPJ_UINT32)getc(IN) << 8;
        header->biGreenGamma |= (OPJ_UINT32)getc(IN) << 16;
        header->biGreenGamma |= (OPJ_UINT32)getc(IN) << 24;

        header->biBlueGamma  = (OPJ_UINT32)getc(IN);
        header->biBlueGamma |= (OPJ_UINT32)getc(IN) << 8;
        header->biBlueGamma |= (OPJ_UINT32)getc(IN) << 16;
        header->biBlueGamma |= (OPJ_UINT32)getc(IN) << 24;
    }

    if (header->biSize >= 124U) {
        header->biIntent  = (OPJ_UINT32)getc(IN);
        header->biIntent |= (OPJ_UINT32)getc(IN) << 8;
        header->biIntent |= (OPJ_UINT32)getc(IN) << 16;
        header->biIntent |= (OPJ_UINT32)getc(IN) << 24;

        header->biIccProfileData  = (OPJ_UINT32)getc(IN);
        header->biIccProfileData |= (OPJ_UINT32)getc(IN) << 8;
        header->biIccProfileData |= (OPJ_UINT32)getc(IN) << 16;
        header->biIccProfileData |= (OPJ_UINT32)getc(IN) << 24;

        header->biIccProfileSize  = (OPJ_UINT32)getc(IN);
        header->biIccProfileSize |= (OPJ_UINT32)getc(IN) << 8;
        header->biIccProfileSize |= (OPJ_UINT32)getc(IN) << 16;
        header->biIccProfileSize |= (OPJ_UINT32)getc(IN) << 24;

        header->biReserved  = (OPJ_UINT32)getc(IN);
        header->biReserved |= (OPJ_UINT32)getc(IN) << 8;
        header->biReserved |= (OPJ_UINT32)getc(IN) << 16;
        header->biReserved |= (OPJ_UINT32)getc(IN) << 24;
    }
    return OPJ_TRUE;
}
