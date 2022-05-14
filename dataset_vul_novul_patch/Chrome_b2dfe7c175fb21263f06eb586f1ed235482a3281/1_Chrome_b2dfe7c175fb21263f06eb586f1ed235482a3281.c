static void _ewk_frame_smart_del(Evas_Object* ewkFrame)
{
    EWK_FRAME_SD_GET(ewkFrame, smartData);

    if (smartData) {
         if (smartData->frame) {
             WebCore::FrameLoaderClientEfl* flc = _ewk_frame_loader_efl_get(smartData->frame);
             flc->setWebFrame(0);
            smartData->frame->loader()->detachFromParent();
            smartData->frame->loader()->cancelAndClear();
             smartData->frame = 0;
         }
 
        eina_stringshare_del(smartData->title);
        eina_stringshare_del(smartData->uri);
        eina_stringshare_del(smartData->name);
    }

    _parent_sc.del(ewkFrame);
}
