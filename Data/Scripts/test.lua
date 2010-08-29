-- test Lua script, that creates CText object, sets its Text and Position through Deku2D engine API calls

mytext = CreateNewText("mytext")
SetText(mytext, "This text is created by Lua script")
SetPosition(mytext, 150, 250)


mytext = CreateNewText("mytext")
SetText(mytext, "Other text")
SetPosition(mytext, 150, 230)