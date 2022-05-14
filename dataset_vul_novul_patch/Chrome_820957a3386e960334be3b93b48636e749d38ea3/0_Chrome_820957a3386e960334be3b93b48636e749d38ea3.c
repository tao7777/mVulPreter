 ColorChooserWin* ColorChooserWin::Open(content::WebContents* web_contents,
                                        SkColor initial_color) {
  if (current_color_chooser_)
    return NULL;
  current_color_chooser_ = new ColorChooserWin(web_contents, initial_color);
   return current_color_chooser_;
 }
