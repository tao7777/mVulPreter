void CrosMock::SetSpeechSynthesisLibraryExpectations() {
  InSequence s;
   EXPECT_CALL(*mock_speech_synthesis_library_, StopSpeaking())
       .WillOnce(Return(true))
       .RetiresOnSaturation();
  EXPECT_CALL(*mock_speech_synthesis_library_, SetSpeakProperties(_))
      .WillOnce(Return(true))
      .RetiresOnSaturation();
   EXPECT_CALL(*mock_speech_synthesis_library_, Speak(_))
       .WillOnce(Return(true))
       .RetiresOnSaturation();
   EXPECT_CALL(*mock_speech_synthesis_library_, IsSpeaking())
      .WillOnce(Return(true))
      .RetiresOnSaturation();
   EXPECT_CALL(*mock_speech_synthesis_library_, StopSpeaking())
       .WillOnce(Return(true))
       .RetiresOnSaturation();
  EXPECT_CALL(*mock_speech_synthesis_library_, SetSpeakProperties(_))
      .WillOnce(Return(true))
      .RetiresOnSaturation();
   EXPECT_CALL(*mock_speech_synthesis_library_, Speak(_))
       .WillOnce(Return(true))
       .RetiresOnSaturation();
  EXPECT_CALL(*mock_speech_synthesis_library_, IsSpeaking())
      .WillOnce(Return(true))
      .WillOnce(Return(true))
      .WillOnce(Return(false))
      .RetiresOnSaturation();
}
