void PrintRenderFrameHelper::PrintHeaderAndFooter(
    blink::WebCanvas* canvas,
    int page_number,
    int total_pages,
    const blink::WebLocalFrame& source_frame,
    float webkit_scale_factor,
    const PageSizeMargins& page_layout,
    const PrintMsg_Print_Params& params) {
  cc::PaintCanvasAutoRestore auto_restore(canvas, true);
  canvas->scale(1 / webkit_scale_factor, 1 / webkit_scale_factor);

  blink::WebSize page_size(page_layout.margin_left + page_layout.margin_right +
                               page_layout.content_width,
                           page_layout.margin_top + page_layout.margin_bottom +
                               page_layout.content_height);

  blink::WebView* web_view = blink::WebView::Create(
      nullptr, blink::mojom::PageVisibilityState::kVisible);
  web_view->GetSettings()->SetJavaScriptEnabled(true);

  class HeaderAndFooterClient final : public blink::WebFrameClient {
   public:
    void BindToFrame(blink::WebLocalFrame* frame) override { frame_ = frame; }
    void FrameDetached(DetachType detach_type) override {
      frame_->FrameWidget()->Close();
      frame_->Close();
      frame_ = nullptr;
    }

   private:
    blink::WebLocalFrame* frame_;
  };
  HeaderAndFooterClient frame_client;
  blink::WebLocalFrame* frame = blink::WebLocalFrame::CreateMainFrame(
      web_view, &frame_client, nullptr, nullptr);
  blink::WebWidgetClient web_widget_client;
  blink::WebFrameWidget::Create(&web_widget_client, frame);

  base::Value html(base::UTF8ToUTF16(
      ui::ResourceBundle::GetSharedInstance().GetRawDataResource(
          IDR_PRINT_PREVIEW_PAGE)));
  ExecuteScript(frame, kPageLoadScriptFormat, html);

  auto options = base::MakeUnique<base::DictionaryValue>();
  options->SetDouble(kSettingHeaderFooterDate, base::Time::Now().ToJsTime());
  options->SetDouble("width", page_size.width);
  options->SetDouble("height", page_size.height);
  options->SetDouble("topMargin", page_layout.margin_top);
  options->SetDouble("bottomMargin", page_layout.margin_bottom);
  options->SetInteger("pageNumber", page_number);
  options->SetInteger("totalPages", total_pages);
   options->SetString("url", params.url);
   base::string16 title = source_frame.GetDocument().Title().Utf16();
   options->SetString("title", title.empty() ? params.title : title);
  options->SetString("headerTemplate", params.header_template);
  options->SetString("footerTemplate", params.footer_template);
 
   ExecuteScript(frame, kPageSetupScriptFormat, *options);
 
  blink::WebPrintParams webkit_params(page_size);
  webkit_params.printer_dpi = GetDPI(&params);

  frame->PrintBegin(webkit_params);
  frame->PrintPage(0, canvas);
  frame->PrintEnd();

  web_view->Close();
}
