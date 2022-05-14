void DelayedExecutor::delayedExecute(const QString &udi)
{
    Solid::Device device(udi);
 
     QString exec = m_service.exec();
     MacroExpander mx(device);
    mx.expandMacros(exec);
 
     KRun::runCommand(exec, QString(), m_service.icon(), 0);
     deleteLater();
}
