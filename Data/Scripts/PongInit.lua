local pongProto = Create("PongProto", "PongProtoInst")

GetObject("PlayerOneScoreTextPlace"):SetPosition(PONG_MARGIN + 40, GetWindowHeight() - 30)
GetObject("PlayerTwoScoreTextPlace"):SetPosition(GetWindowWidth() - PONG_MARGIN - 60, GetWindowHeight() - 30)

Root:Attach(pongProto)

DebugPrintComponentTree()
