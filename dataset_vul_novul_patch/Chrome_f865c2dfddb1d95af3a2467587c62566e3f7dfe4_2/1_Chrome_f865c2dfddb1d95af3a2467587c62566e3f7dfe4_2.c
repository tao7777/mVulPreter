DevToolsClient::DevToolsClient(
    RenderFrame* main_render_frame,
    const std::string& compatibility_script)
    : RenderFrameObserver(main_render_frame),
       compatibility_script_(compatibility_script),
       web_tools_frontend_(
           WebDevToolsFrontend::create(main_render_frame->GetWebFrame(), this)) {
 }
