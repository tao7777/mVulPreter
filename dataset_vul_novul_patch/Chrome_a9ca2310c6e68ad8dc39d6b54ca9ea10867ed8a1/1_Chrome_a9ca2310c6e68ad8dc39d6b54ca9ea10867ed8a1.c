 static blink::WebScreenOrientations stringToOrientations(const AtomicString& orientationString)
 {
     DEFINE_STATIC_LOCAL(const AtomicString, portrait, ("portrait", AtomicString::ConstructFromLiteral));
     DEFINE_STATIC_LOCAL(const AtomicString, landscape, ("landscape", AtomicString::ConstructFromLiteral));
 
     if (orientationString == portrait)
         return blink::WebScreenOrientationPortraitPrimary | blink::WebScreenOrientationPortraitSecondary;
     if (orientationString == landscape)
        return blink::WebScreenOrientationLandscapePrimary | blink::WebScreenOrientationLandscapeSecondary;

    unsigned length = 0;
    ScreenOrientationInfo* orientationMap = orientationsMap(length);
    for (unsigned i = 0; i < length; ++i) {
        if (orientationMap[i].name == orientationString)
            return orientationMap[i].orientation;
    }
    return 0;
}
