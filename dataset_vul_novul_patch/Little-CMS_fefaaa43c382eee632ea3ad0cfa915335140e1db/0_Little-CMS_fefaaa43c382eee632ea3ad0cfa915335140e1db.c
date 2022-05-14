cmsPipeline* DefaultICCintents(cmsContext       ContextID,
                               cmsUInt32Number  nProfiles,
                               cmsUInt32Number  TheIntents[],
                               cmsHPROFILE      hProfiles[],
                               cmsBool          BPC[],
                               cmsFloat64Number AdaptationStates[],
                               cmsUInt32Number  dwFlags)
{
    cmsPipeline* Lut = NULL;
    cmsPipeline* Result;
    cmsHPROFILE hProfile;
    cmsMAT3 m;
    cmsVEC3 off;
    cmsColorSpaceSignature ColorSpaceIn, ColorSpaceOut, CurrentColorSpace;
    cmsProfileClassSignature ClassSig;
    cmsUInt32Number  i, Intent;

    if (nProfiles == 0) return NULL;

    Result = cmsPipelineAlloc(ContextID, 0, 0);
    if (Result == NULL) return NULL;

    CurrentColorSpace = cmsGetColorSpace(hProfiles[0]);

    for (i=0; i < nProfiles; i++) {

        cmsBool  lIsDeviceLink, lIsInput;

        hProfile      = hProfiles[i];
        ClassSig      = cmsGetDeviceClass(hProfile);
        lIsDeviceLink = (ClassSig == cmsSigLinkClass || ClassSig == cmsSigAbstractClass );

        if ((i == 0) && !lIsDeviceLink) {
            lIsInput = TRUE;
        }
        else {
        lIsInput      = (CurrentColorSpace != cmsSigXYZData) &&
                        (CurrentColorSpace != cmsSigLabData);
        }

        Intent        = TheIntents[i];

        if (lIsInput || lIsDeviceLink) {

            ColorSpaceIn    = cmsGetColorSpace(hProfile);
            ColorSpaceOut   = cmsGetPCS(hProfile);
        }
        else {

            ColorSpaceIn    = cmsGetPCS(hProfile);
            ColorSpaceOut   = cmsGetColorSpace(hProfile);
        }

        if (!ColorSpaceIsCompatible(ColorSpaceIn, CurrentColorSpace)) {

            cmsSignalError(ContextID, cmsERROR_COLORSPACE_CHECK, "ColorSpace mismatch");
            goto Error;
        }

        if (lIsDeviceLink || ((ClassSig == cmsSigNamedColorClass) && (nProfiles == 1))) {

            Lut = _cmsReadDevicelinkLUT(hProfile, Intent);
            if (Lut == NULL) goto Error;

             if (ClassSig == cmsSigAbstractClass && i > 0) {
                if (!ComputeConversion(i, hProfiles, Intent, BPC[i], AdaptationStates[i], &m, &off)) goto Error;
             }
             else {
                _cmsMAT3identity(&m);
                _cmsVEC3init(&off, 0, 0, 0);
             }


            if (!AddConversion(Result, CurrentColorSpace, ColorSpaceIn, &m, &off)) goto Error;

        }
        else {

            if (lIsInput) {
                Lut = _cmsReadInputLUT(hProfile, Intent);
                if (Lut == NULL) goto Error;
            }
            else {

                Lut = _cmsReadOutputLUT(hProfile, Intent);
                if (Lut == NULL) goto Error;


                if (!ComputeConversion(i, hProfiles, Intent, BPC[i], AdaptationStates[i], &m, &off)) goto Error;
                if (!AddConversion(Result, CurrentColorSpace, ColorSpaceIn, &m, &off)) goto Error;

            }
        }

         if (!cmsPipelineCat(Result, Lut))
             goto Error;

         cmsPipelineFree(Lut);
        Lut = NULL;
 
         CurrentColorSpace = ColorSpaceOut;
    }

    return Result;
 
 Error:
 
    if (Lut != NULL) cmsPipelineFree(Lut);
     if (Result != NULL) cmsPipelineFree(Result);
     return NULL;
 
    cmsUNUSED_PARAMETER(dwFlags);
}
