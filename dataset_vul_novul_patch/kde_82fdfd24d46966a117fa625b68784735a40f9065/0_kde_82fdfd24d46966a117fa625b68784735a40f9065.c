void Part::slotOpenExtractedEntry(KJob *job)
{
    if (!job->error()) {

        OpenJob *openJob = qobject_cast<OpenJob*>(job);
        Q_ASSERT(openJob);

        m_tmpExtractDirList << openJob->tempDir();

        const QString fullName = openJob->validatedFilePath();

        bool isWritable = m_model->archive() && !m_model->archive()->isReadOnly();

        if (!isWritable) {
            QFile::setPermissions(fullName, QFileDevice::ReadOwner | QFileDevice::ReadGroup | QFileDevice::ReadOther);
        }

        if (isWritable) {
            m_fileWatcher = new QFileSystemWatcher;
            connect(m_fileWatcher, &QFileSystemWatcher::fileChanged, this, &Part::slotWatchedFileModified);
            m_fileWatcher->addPath(fullName);
        }

        if (qobject_cast<OpenWithJob*>(job)) {
            const QList<QUrl> urls = {QUrl::fromUserInput(fullName, QString(), QUrl::AssumeLocalFile)};
            KRun::displayOpenWithDialog(urls, widget());
         } else {
             KRun::runUrl(QUrl::fromUserInput(fullName, QString(), QUrl::AssumeLocalFile),
                          QMimeDatabase().mimeTypeForFile(fullName).name(),
                         widget(), false, false);
         }
     } else if (job->error() != KJob::KilledJobError) {
         KMessageBox::error(widget(), job->errorString());
    }
    setReadyGui();
}
