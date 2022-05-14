 void ScrollHitTestDisplayItem::PropertiesAsJSON(JSONObject& json) const {
   DisplayItem::PropertiesAsJSON(json);
   json.SetString("scrollOffsetNode",
                 String::Format("%p", scroll_offset_node_.get()));
 }
