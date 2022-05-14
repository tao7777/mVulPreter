void SerializerMarkupAccumulator::appendText(StringBuilder& result, Text* text)
void SerializerMarkupAccumulator::appendText(StringBuilder& out, Text* text)
 {
     Element* parent = text->parentElement();
     if (parent && !shouldIgnoreElement(parent))
        MarkupAccumulator::appendText(out, text);
 }
