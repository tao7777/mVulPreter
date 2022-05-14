 PrintPreviewMessageHandler::PrintPreviewMessageHandler(
     WebContents* web_contents)
    : content::WebContentsObserver(web_contents), weak_ptr_factory_(this) {
   DCHECK(web_contents);
 }
