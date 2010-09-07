-- test Lua script, that creates CText object, sets its Text and Position through Deku2D engine API calls

mytext = CreateNewText("mytext")
SetText(mytext, "This text is created by Lua script")
SetPosition(mytext, 150, 250)


mytext = CreateNewText("mytext2")
SetText(mytext, "Other text - now in other font")
SetPosition(mytext, 150, 220)
SetTextFont(mytext, GetFont("hge"))
