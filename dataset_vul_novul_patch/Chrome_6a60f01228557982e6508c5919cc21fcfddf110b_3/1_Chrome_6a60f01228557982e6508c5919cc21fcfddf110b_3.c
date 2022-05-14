 FrameImpl::FrameImpl(std::unique_ptr<content::WebContents> web_contents,
                     chromium::web::FrameObserverPtr observer)
    : web_contents_(std::move(web_contents)), observer_(std::move(observer)) {
  Observe(web_contents.get());
 }
