JSGlobalContextRef webkit_web_frame_get_global_context(WebKitWebFrame* frame)
{
    g_return_val_if_fail(WEBKIT_IS_WEB_FRAME(frame), NULL);

    Frame* coreFrame = core(frame);
     if (!coreFrame)
         return NULL;
 
    return toGlobalRef(coreFrame->script()->globalObject()->globalExec());
 }
