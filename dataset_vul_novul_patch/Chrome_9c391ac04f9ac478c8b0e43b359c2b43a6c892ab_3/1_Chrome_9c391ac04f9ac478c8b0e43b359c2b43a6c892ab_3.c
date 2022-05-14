 PrintPreviewMessageHandler::PrintPreviewMessageHandler(
     WebContents* web_contents)
    : content::WebContentsObserver(web_contents) {
   DCHECK(web_contents);
 }
