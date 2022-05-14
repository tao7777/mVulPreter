static void _ewk_frame_smart_del(Evas_Object* ewkFrame)
{
    EWK_FRAME_SD_GET(ewkFrame, smartData);

    if (smartData) {
         if (smartData->frame) {
             WebCore::FrameLoaderClientEfl* flc = _ewk_frame_loader_efl_get(smartData->frame);
             flc->setWebFrame(0);
            EWK_FRAME_SD_GET(ewk_view_frame_main_get(smartData->view), mainSmartData);
            if (mainSmartData->frame == smartData->frame) // applying only for main frame is enough (will traverse through frame tree)
                smartData->frame->loader()->detachFromParent();
             smartData->frame = 0;
         }
 
        eina_stringshare_del(smartData->title);
        eina_stringshare_del(smartData->uri);
        eina_stringshare_del(smartData->name);
    }

    _parent_sc.del(ewkFrame);
}
