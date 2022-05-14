 static void handleMetaReferrer(const String& attributeValue, CachedDocumentParameters* documentParameters, CSSPreloadScanner* cssScanner)
 {
    ReferrerPolicy metaReferrerPolicy = ReferrerPolicyDefault;
    if (!attributeValue.isEmpty() && !attributeValue.isNull() && SecurityPolicy::referrerPolicyFromString(attributeValue, &metaReferrerPolicy)) {
        documentParameters->referrerPolicy = metaReferrerPolicy;
     }
     cssScanner->setReferrerPolicy(documentParameters->referrerPolicy);
 }
