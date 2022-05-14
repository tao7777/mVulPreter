 ExtensionInfoBar::ExtensionInfoBar(ExtensionInfoBarDelegate* delegate)
    : InfoBarView(delegate),
       delegate_(delegate),
       ALLOW_THIS_IN_INITIALIZER_LIST(tracker_(this)) {
   delegate_->set_observer(this);

  ExtensionHost* extension_host = delegate_->extension_host();

  gfx::Size sz = extension_host->view()->GetPreferredSize();
  if (sz.height() > 0)
    sz.set_height(sz.height() + 1);
  set_target_height(sz.height());

  SetupIconAndMenu();

  extension_host->view()->SetContainer(this);
  extension_host->view()->set_parent_owned(false);
  AddChildView(extension_host->view());
}
