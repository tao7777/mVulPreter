HeadlessWebContentsImpl::HeadlessWebContentsImpl(
    content::WebContents* web_contents,
    HeadlessBrowserContextImpl* browser_context)
    : content::WebContentsObserver(web_contents),
      web_contents_delegate_(new HeadlessWebContentsImpl::Delegate(this)),
      web_contents_(web_contents),
      agent_host_(content::DevToolsAgentHost::GetOrCreateFor(web_contents)),
      inject_mojo_services_into_isolated_world_(false),
      browser_context_(browser_context),
      render_process_host_(web_contents->GetMainFrame()->GetProcess()),
       weak_ptr_factory_(this) {
 #if BUILDFLAG(ENABLE_BASIC_PRINTING) && !defined(CHROME_MULTIPLE_DLL_CHILD)
   HeadlessPrintManager::CreateForWebContents(web_contents);
//// TODO(weili): Add support for printing OOPIFs.
 #endif
   web_contents->GetMutableRendererPrefs()->accept_languages =
       browser_context->options()->accept_language();
  web_contents_->SetDelegate(web_contents_delegate_.get());
  render_process_host_->AddObserver(this);
  agent_host_->AddObserver(this);
}
