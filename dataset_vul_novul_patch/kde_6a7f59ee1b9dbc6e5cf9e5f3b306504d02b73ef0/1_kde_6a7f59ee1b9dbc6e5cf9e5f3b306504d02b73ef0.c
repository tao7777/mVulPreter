QString IRCView::closeToTagString(TextHtmlData* data, const QString& _tag)
{
    QString ret;
    QString tag;
    int i = data->openHtmlTags.count() - 1;
    for ( ; i >= 0 ; --i)
    {
        tag = data->openHtmlTags.at(i);
        ret += QLatin1String("</") + tag + QLatin1Char('>');
        if (tag == _tag)
        {
            data->openHtmlTags.removeAt(i);
            break;
        }
     }
 
    ret += openTags(data, i);
 
     return ret;
 }
