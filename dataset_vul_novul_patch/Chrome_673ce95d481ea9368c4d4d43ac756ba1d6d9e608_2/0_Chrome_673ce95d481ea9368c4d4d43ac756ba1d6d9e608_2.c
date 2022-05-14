bool PrintRenderFrameHelper::PreviewPageRendered(int page_number,
                                                 PdfMetafileSkia* metafile) {
  DCHECK_GE(page_number, FIRST_PAGE_INDEX);

  if (!print_preview_context_.IsModifiable() ||
      !print_preview_context_.generate_draft_pages()) {
    DCHECK(!metafile);
    return true;
  }

  if (!metafile) {
    NOTREACHED();
    print_preview_context_.set_error(
        PREVIEW_ERROR_PAGE_RENDERED_WITHOUT_METAFILE);
    return false;
   }
 
   PrintHostMsg_DidPreviewPage_Params preview_page_params;
  if (!CopyMetafileDataToReadOnlySharedMem(
          *metafile, &preview_page_params.metafile_data_handle)) {
    LOG(ERROR) << "CopyMetafileDataToReadOnlySharedMem failed";
     print_preview_context_.set_error(PREVIEW_ERROR_METAFILE_COPY_FAILED);
     return false;
   }

  preview_page_params.data_size = metafile->GetDataSize();
  preview_page_params.page_number = page_number;
  preview_page_params.preview_request_id =
      print_pages_params_->params.preview_request_id;

  Send(new PrintHostMsg_DidPreviewPage(routing_id(), preview_page_params));
  return true;
}
