xsltNumber(xsltTransformContextPtr ctxt, xmlNodePtr node,
	   xmlNodePtr inst, xsltStylePreCompPtr castedComp)
{
#ifdef XSLT_REFACTORED
    xsltStyleItemNumberPtr comp = (xsltStyleItemNumberPtr) castedComp;
 #else
     xsltStylePreCompPtr comp = castedComp;
 #endif
     if (comp == NULL) {
 	xsltTransformError(ctxt, NULL, inst,
 	     "xsl:number : compilation failed\n");
	return;
    }

    if ((ctxt == NULL) || (node == NULL) || (inst == NULL) || (comp == NULL))
	return;

     comp->numdata.doc = inst->doc;
     comp->numdata.node = inst;
 
     xsltNumberFormat(ctxt, &comp->numdata, node);
 }
