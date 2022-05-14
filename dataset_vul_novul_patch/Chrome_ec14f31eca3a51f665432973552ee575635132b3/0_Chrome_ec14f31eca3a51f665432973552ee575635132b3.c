Eina_Bool ewk_view_scale_set(Evas_Object* ewkView, float scaleFactor, Evas_Coord centerX, Evas_Coord centerY)
Eina_Bool ewk_view_scale_set(Evas_Object* ewkView, float scaleFactor, Evas_Coord scrollX, Evas_Coord scrollY)
 {
     EWK_VIEW_SD_GET_OR_RETURN(ewkView, smartData, false);
     EWK_VIEW_PRIV_GET_OR_RETURN(smartData, priv, false);

    float currentScaleFactor = ewk_view_scale_get(ewkView);
     if (currentScaleFactor == -1)
         return false;
 
    priv->page->setPageScaleFactor(scaleFactor, WebCore::LayoutPoint(scrollX, scrollY));
     return true;
 }
