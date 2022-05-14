 WebString WebPageSerializer::generateMarkOfTheWebDeclaration(const WebURL& url)
 {
    return String::format("\n<!-- saved from url=(%04d)%s -->\n",
                          static_cast<int>(url.spec().length()),
                          url.spec().data());
 }
