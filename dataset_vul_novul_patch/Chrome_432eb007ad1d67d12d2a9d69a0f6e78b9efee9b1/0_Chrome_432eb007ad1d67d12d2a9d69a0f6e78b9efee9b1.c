void OpenPDFInReaderView::Update(content::WebContents* web_contents) {
  model_ = NULL;
  if (web_contents) {
    pdf::PDFWebContentsHelper* pdf_tab_helper =
        pdf::PDFWebContentsHelper::FromWebContents(web_contents);
    model_ = pdf_tab_helper->open_in_reader_prompt();
   }
 
   SetVisible(!!model_);

  // Hide the bubble if it is currently shown and the icon is hidden.
  if (!model_ && bubble_)
    bubble_->GetWidget()->Hide();
 }
