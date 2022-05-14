Eina_Bool ewk_view_scale_set(Evas_Object* ewkView, float scaleFactor, Evas_Coord centerX, Evas_Coord centerY)
 {
     EWK_VIEW_SD_GET_OR_RETURN(ewkView, smartData, false);
     EWK_VIEW_PRIV_GET_OR_RETURN(smartData, priv, false);

    float currentScaleFactor = ewk_view_scale_get(ewkView);
     if (currentScaleFactor == -1)
         return false;
 
    int x, y;
    ewk_frame_scroll_pos_get(smartData->main_frame, &x, &y);
    x = static_cast<int>(((x + centerX) / currentScaleFactor) * scaleFactor) - centerX;
    y = static_cast<int>(((y + centerY) / currentScaleFactor) * scaleFactor) - centerY;
    priv->page->setPageScaleFactor(scaleFactor, WebCore::LayoutPoint(x, y));
     return true;
 }
