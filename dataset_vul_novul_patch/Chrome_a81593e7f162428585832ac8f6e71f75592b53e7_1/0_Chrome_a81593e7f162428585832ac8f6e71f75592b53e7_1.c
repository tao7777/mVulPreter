 WebString WebPageSerializer::generateMarkOfTheWebDeclaration(const WebURL& url)
 {
    StringBuilder builder;
    builder.append("\n<!-- ");
    builder.append(PageSerializer::markOfTheWebDeclaration(url));
    builder.append(" -->\n");
    return builder.toString();
 }
