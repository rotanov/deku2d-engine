Create("PlaceableComponent", "PlayerOnePlace")
Create("PlaceableComponent", "PlayerTwoPlace")
Create("PlaceableComponent", "BallPlace")

Create("PlaceableComponent", "PlayerOneScoreTextPlace")
Create("PlaceableComponent", "PlayerTwoScoreTextPlace")
Create("Text", "PlayerOneScoreText")
Create("Text", "PlayerTwoScoreText")

Attach(GetObject("PlayerOneScoreTextPlace"), GetObject("PlayerOneScoreText"))
Attach(GetObject("PlayerTwoScoreTextPlace"), GetObject("PlayerTwoScoreText"))

Create("ScriptableComponent", "PlayerOneScriptable")
Create("ScriptableComponent", "PlayerTwoScriptable")
Create("ScriptableComponent", "BallScriptable")
Create("ScriptableComponent", "PongGame")

SetScript(GetObject("PlayerOneScriptable"), GetObject("PongPlayers"))
SetScript(GetObject("PlayerTwoScriptable"), GetObject("PongPlayers"))
SetScript(GetObject("BallScriptable"), GetObject("BallScript"))
SetScript(GetObject("PongGame"), GetObject("PongGameScript"))

Attach(GetObject("PlayerOnePlace"), GetObject("PlayerOne"))
Attach(GetObject("PlayerTwoPlace"), GetObject("PlayerTwo"))
Attach(GetObject("BallPlace"), GetObject("Ball"))

Attach(GetObject("PlayerOne"), GetObject("PlayerOneScriptable"))
Attach(GetObject("PlayerTwo"), GetObject("PlayerTwoScriptable"))
Attach(GetObject("Ball"), GetObject("BallScriptable"))

Attach(GetObject("RootGameObject"), GetObject("PlayerOnePlace"))
Attach(GetObject("RootGameObject"), GetObject("PlayerTwoPlace"))
Attach(GetObject("RootGameObject"), GetObject("BallPlace"))
Attach(GetObject("RootGameObject"), GetObject("PlayerOneScoreTextPlace"))
Attach(GetObject("RootGameObject"), GetObject("PlayerTwoScoreTextPlace"))

SetPosition(GetObject("PlayerOneScoreTextPlace"), PONG_MARGIN + 40, GetWindowHeight() - 30)
SetPosition(GetObject("PlayerTwoScoreTextPlace"), GetWindowWidth() - PONG_MARGIN - 60, GetWindowHeight() - 30)
SetText(GetObject("PlayerOneScoreText"), "0")
SetText(GetObject("PlayerTwoScoreText"), "0")
SetTextFont(GetObject("PlayerOneScoreText"), GetObject("hge"))
SetTextFont(GetObject("PlayerTwoScoreText"), GetObject("hge"))
