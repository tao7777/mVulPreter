 ui::ModalType ExtensionInstallDialogView::GetModalType() const {
  return prompt_->ShouldUseTabModalDialog() ? ui::MODAL_TYPE_CHILD
                                            : ui::MODAL_TYPE_WINDOW;
 }
