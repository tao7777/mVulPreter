Curves16Data* CurvesAlloc(cmsContext ContextID, int nCurves, int nElements, cmsToneCurve** G)
{
    int i, j;
    Curves16Data* c16;

    c16 = _cmsMallocZero(ContextID, sizeof(Curves16Data));
    if (c16 == NULL) return NULL;

    c16 ->nCurves = nCurves;
    c16 ->nElements = nElements;

    c16 ->Curves = _cmsCalloc(ContextID, nCurves, sizeof(cmsUInt16Number*));
    if (c16 ->Curves == NULL) return NULL;

    for (i=0; i < nCurves; i++) {
 
         c16->Curves[i] = _cmsCalloc(ContextID, nElements, sizeof(cmsUInt16Number));
 
        if (c16->Curves[i] == NULL) {

            for (j=0; j < i; j++) {
                _cmsFree(ContextID, c16->Curves[j]);
            }
            _cmsFree(ContextID, c16->Curves);
            _cmsFree(ContextID, c16);
            return NULL;
        }

         if (nElements == 256) {
 
             for (j=0; j < nElements; j++) {

                c16 ->Curves[i][j] = cmsEvalToneCurve16(G[i], FROM_8_TO_16(j));
            }
        }
        else {

            for (j=0; j < nElements; j++) {
                c16 ->Curves[i][j] = cmsEvalToneCurve16(G[i], (cmsUInt16Number) j);
            }
        }
    }

    return c16;
}
