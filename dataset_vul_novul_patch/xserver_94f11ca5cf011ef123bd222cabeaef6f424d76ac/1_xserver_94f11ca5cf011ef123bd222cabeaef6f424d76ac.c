 tbGetBuffer(unsigned size)
 {
    char *rtrn;
 
    if (size >= BUFFER_SIZE)
        return NULL;
    if ((BUFFER_SIZE - tbNext) <= size)
        tbNext = 0;
    rtrn = &textBuffer[tbNext];
    tbNext += size;
    return rtrn;
 }
