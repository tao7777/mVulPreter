CSSStyleSheet* CSSStyleSheet::CreateInline(Node& owner_node,
                                           const KURL& base_url,
                                           const TextPosition& start_position,
                                            const WTF::TextEncoding& encoding) {
   CSSParserContext* parser_context = CSSParserContext::Create(
       owner_node.GetDocument(), owner_node.GetDocument().BaseURL(),
      false /* is_opaque_response_from_service_worker */,
       owner_node.GetDocument().GetReferrerPolicy(), encoding);
   StyleSheetContents* sheet =
       StyleSheetContents::Create(base_url.GetString(), parser_context);
  return new CSSStyleSheet(sheet, owner_node, true, start_position);
}
