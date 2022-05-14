void NavigateOnUIThread(
void NavigateOnUIThread(const GURL& url,
                        const std::vector<GURL> url_chain,
                        const Referrer& referrer,
                        bool has_user_gesture,
                        const ResourceRequestInfo::WebContentsGetter& wc_getter,
                        int frame_tree_node_id) {
   DCHECK_CURRENTLY_ON(BrowserThread::UI);
 
   WebContents* web_contents = wc_getter.Run();
  if (web_contents) {
    NavigationController::LoadURLParams params(url);
     params.has_user_gesture = has_user_gesture;
     params.referrer = referrer;
     params.redirect_chain = url_chain;
    params.frame_tree_node_id = frame_tree_node_id;
     web_contents->GetController().LoadURLWithParams(params);
   }
 }
