void OpenPDFInReaderView::Update(content::WebContents* web_contents) {
  model_ = NULL;
  if (web_contents) {
    pdf::PDFWebContentsHelper* pdf_tab_helper =
        pdf::PDFWebContentsHelper::FromWebContents(web_contents);
    model_ = pdf_tab_helper->open_in_reader_prompt();
   }
 
   SetVisible(!!model_);
 }
