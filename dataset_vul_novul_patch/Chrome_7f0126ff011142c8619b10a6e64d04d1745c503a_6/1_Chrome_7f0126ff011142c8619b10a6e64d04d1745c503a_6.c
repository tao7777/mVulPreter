void SerializerMarkupAccumulator::appendText(StringBuilder& result, Text* text)
 {
     Element* parent = text->parentElement();
     if (parent && !shouldIgnoreElement(parent))
        MarkupAccumulator::appendText(result, text);
 }
