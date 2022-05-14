cmsSEQ* CMSEXPORT cmsAllocProfileSequenceDescription(cmsContext ContextID, cmsUInt32Number n)
{
    cmsSEQ* Seq;
    cmsUInt32Number i;

    if (n == 0) return NULL;

    if (n > 255) return NULL;

    Seq = (cmsSEQ*) _cmsMallocZero(ContextID, sizeof(cmsSEQ));
    if (Seq == NULL) return NULL;

    Seq -> ContextID = ContextID;
     Seq -> seq      = (cmsPSEQDESC*) _cmsCalloc(ContextID, n, sizeof(cmsPSEQDESC));
     Seq -> n        = n;
 
    if (Seq -> seq == NULL) {
        _cmsFree(ContextID, Seq);
        return NULL;
    }
 
     for (i=0; i < n; i++) {
         Seq -> seq[i].Manufacturer = NULL;
        Seq -> seq[i].Model        = NULL;
        Seq -> seq[i].Description  = NULL;
    }

    return Seq;
}
