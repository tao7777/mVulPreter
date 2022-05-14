void faad_resetbits(bitfile *ld, int bits)
{
    uint32_t tmp;
     int words = bits >> 5;
     int remainder = bits & 0x1F;
 
    if (ld->buffer_size < words * 4)
        ld->bytes_left = 0;
    else
        ld->bytes_left = ld->buffer_size - words*4;
 
     if (ld->bytes_left >= 4)
     {
        tmp = getdword(&ld->start[words]);
        ld->bytes_left -= 4;
    } else {
        tmp = getdword_n(&ld->start[words], ld->bytes_left);
        ld->bytes_left = 0;
    }
    ld->bufa = tmp;

    if (ld->bytes_left >= 4)
    {
        tmp = getdword(&ld->start[words+1]);
        ld->bytes_left -= 4;
    } else {
        tmp = getdword_n(&ld->start[words+1], ld->bytes_left);
        ld->bytes_left = 0;
    }
    ld->bufb = tmp;

    ld->bits_left = 32 - remainder;
    ld->tail = &ld->start[words+2];

    /* recheck for reading too many bytes */
    ld->error = 0;
}
