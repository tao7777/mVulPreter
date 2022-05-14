 bool InputWindowInfo::isTrustedOverlay() const {
     return layoutParamsType == TYPE_INPUT_METHOD
             || layoutParamsType == TYPE_INPUT_METHOD_DIALOG
             || layoutParamsType == TYPE_MAGNIFICATION_OVERLAY
            || layoutParamsType == TYPE_STATUS_BAR
            || layoutParamsType == TYPE_NAVIGATION_BAR
             || layoutParamsType == TYPE_SECURE_SYSTEM_OVERLAY;
 }
