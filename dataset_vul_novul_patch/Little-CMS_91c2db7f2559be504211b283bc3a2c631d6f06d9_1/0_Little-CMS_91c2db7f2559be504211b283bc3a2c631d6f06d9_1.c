cmsBool  CMSEXPORT cmsAppendNamedColor(cmsNAMEDCOLORLIST* NamedColorList,
                                       const char* Name,
                                       cmsUInt16Number PCS[3], cmsUInt16Number Colorant[cmsMAXCHANNELS])
{
    cmsUInt32Number i;

    if (NamedColorList == NULL) return FALSE;

    if (NamedColorList ->nColors + 1 > NamedColorList ->Allocated) {
        if (!GrowNamedColorList(NamedColorList)) return FALSE;
    }

    for (i=0; i < NamedColorList ->ColorantCount; i++)
        NamedColorList ->List[NamedColorList ->nColors].DeviceColorant[i] = Colorant == NULL? 0 : Colorant[i];

    for (i=0; i < 3; i++)
        NamedColorList ->List[NamedColorList ->nColors].PCS[i] = PCS == NULL ? 0 : PCS[i];
 
     if (Name != NULL) {
 
        strncpy(NamedColorList ->List[NamedColorList ->nColors].Name, Name, cmsMAX_PATH-1);
         NamedColorList ->List[NamedColorList ->nColors].Name[cmsMAX_PATH-1] = 0;
 
     }
    else
        NamedColorList ->List[NamedColorList ->nColors].Name[0] = 0;


    NamedColorList ->nColors++;
    return TRUE;
}
