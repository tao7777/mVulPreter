void SpeechSynthesis::handleSpeakingCompleted(SpeechSynthesisUtterance* utterance, bool errorOccurred)
 {
     ASSERT(utterance);
 
    // Keep the utterance around long enough to fire an event on it in case m_utteranceQueue
    // is holding the last reference to it.
    RefPtrWillBeRawPtr<SpeechSynthesisUtterance> protect(utterance);

     bool didJustFinishCurrentUtterance = false;
    if (utterance == currentSpeechUtterance()) {
        m_utteranceQueue.removeFirst();
        didJustFinishCurrentUtterance = true;
    }

    fireEvent(errorOccurred ? EventTypeNames::error : EventTypeNames::end, utterance, 0, String());

    if (didJustFinishCurrentUtterance && !m_utteranceQueue.isEmpty())
        startSpeakingImmediately();
}
