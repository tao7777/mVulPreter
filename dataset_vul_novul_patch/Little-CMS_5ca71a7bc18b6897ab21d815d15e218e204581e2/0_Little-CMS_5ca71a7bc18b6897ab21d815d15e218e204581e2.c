void *Type_MLU_Read(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, cmsUInt32Number* nItems, cmsUInt32Number SizeOfTag)
{
    cmsMLU* mlu;
    cmsUInt32Number Count, RecLen, NumOfWchar;
    cmsUInt32Number SizeOfHeader;
    cmsUInt32Number  Len, Offset;
    cmsUInt32Number  i;
    wchar_t*         Block;
    cmsUInt32Number  BeginOfThisString, EndOfThisString, LargestPosition;

    *nItems = 0;
    if (!_cmsReadUInt32Number(io, &Count)) return NULL;
    if (!_cmsReadUInt32Number(io, &RecLen)) return NULL;

    if (RecLen != 12) {

        cmsSignalError(self->ContextID, cmsERROR_UNKNOWN_EXTENSION, "multiLocalizedUnicodeType of len != 12 is not supported.");
        return NULL;
    }

    mlu = cmsMLUalloc(self ->ContextID, Count);
    if (mlu == NULL) return NULL;

    mlu ->UsedEntries = Count;

    SizeOfHeader = 12 * Count + sizeof(_cmsTagBase);
    LargestPosition = 0;

    for (i=0; i < Count; i++) {

        if (!_cmsReadUInt16Number(io, &mlu ->Entries[i].Language)) goto Error;
        if (!_cmsReadUInt16Number(io, &mlu ->Entries[i].Country))  goto Error;

        if (!_cmsReadUInt32Number(io, &Len)) goto Error;
        if (!_cmsReadUInt32Number(io, &Offset)) goto Error;
 
         if (Offset < (SizeOfHeader + 8)) goto Error;
        if ((Offset + Len) > SizeOfTag + 8) goto Error;
 
         BeginOfThisString = Offset - SizeOfHeader - 8;

        mlu ->Entries[i].Len = (Len * sizeof(wchar_t)) / sizeof(cmsUInt16Number);
        mlu ->Entries[i].StrW = (BeginOfThisString * sizeof(wchar_t)) / sizeof(cmsUInt16Number);

        EndOfThisString = BeginOfThisString + Len;
        if (EndOfThisString > LargestPosition)
            LargestPosition = EndOfThisString;
    }

    SizeOfTag   = (LargestPosition * sizeof(wchar_t)) / sizeof(cmsUInt16Number);
    if (SizeOfTag == 0)
    {
        Block = NULL;
        NumOfWchar = 0;

    }
    else
    {
        Block = (wchar_t*) _cmsMalloc(self ->ContextID, SizeOfTag);
        if (Block == NULL) goto Error;
        NumOfWchar = SizeOfTag / sizeof(wchar_t);
        if (!_cmsReadWCharArray(io, NumOfWchar, Block)) goto Error;
    }

    mlu ->MemPool  = Block;
    mlu ->PoolSize = SizeOfTag;
    mlu ->PoolUsed = SizeOfTag;

    *nItems = 1;
    return (void*) mlu;

Error:
    if (mlu) cmsMLUfree(mlu);
    return NULL;
}
