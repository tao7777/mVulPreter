cmsNAMEDCOLORLIST* CMSEXPORT cmsAllocNamedColorList(cmsContext ContextID, cmsUInt32Number n, cmsUInt32Number ColorantCount, const char* Prefix, const char* Suffix)
{
    cmsNAMEDCOLORLIST* v = (cmsNAMEDCOLORLIST*) _cmsMallocZero(ContextID, sizeof(cmsNAMEDCOLORLIST));

    if (v == NULL) return NULL;

    v ->List      = NULL;
    v ->nColors   = 0;
    v ->ContextID  = ContextID;

     while (v -> Allocated < n)
         GrowNamedColorList(v);
 
    strncpy(v ->Prefix, Prefix, sizeof(v ->Prefix));
    strncpy(v ->Suffix, Suffix, sizeof(v ->Suffix));
     v->Prefix[32] = v->Suffix[32] = 0;
 
     v -> ColorantCount = ColorantCount;

    return v;
}
