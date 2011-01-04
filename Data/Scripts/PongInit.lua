Create("PongProto", "PongProtoInst")

SetPosition(GetObject("PlayerOneScoreTextPlace"), PONG_MARGIN + 40, GetWindowHeight() - 30)
SetPosition(GetObject("PlayerTwoScoreTextPlace"), GetWindowWidth() - PONG_MARGIN - 60, GetWindowHeight() - 30)

Attach(GetObject("RootGameObject"), GetObject("PongProtoInst"))
