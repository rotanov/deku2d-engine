--[[for i = 0, 3 do
	local a = Create("A", "")
	a.pos:SetPosition(0, 0)
	local nextA = a
	while nextA do
		nextA.pos:SetAngle( 90 * i )
		nextA = nextA.A
	end	
	GetObject("Mouse cursor").pos:Attach(a)
end]]

local lpc = Create("LuaPerfCounter", "")
Root:Attach(lpc)

--[[local ninja = Create("BlackNinja", "")
ninja.pos:SetPosition(320, 240)
Root:Attach(ninja)
]]

--[[for i = 0, 10 do
	local ninja = Create("BlackNinja", "")
	ninja.pos:SetPosition(i * 50, 240)
	Root:Attach(ninja)
end

for i = 0, 10 do
	local ninja = Create("BlackNinja", "")
	ninja.pos:SetPosition(i * 50, 100)
	Root:Attach(ninja)
end

for i = 0, 10 do
	local ninja = Create("BlackNinja", "")
	ninja.pos:SetPosition(i * 50, 380)
	Root:Attach(ninja)
end]]

--local another_ninja = Create("BlackNinja", "")
--another_ninja.pos:SetPosition(350, 100)
--Root:Attach(another_ninja)

--Root:Attach(Create("SandboxProto", "SandboxInst"))
--Attach(GetParent(GetObject("Mouse cursor")), GetObject("Magic square"))

for i = 0, 10 do
	local box = Create("TestBox", "")
	box.pos:SetPosition(i * 40, 200)
	box.model:SetColor(Random_Float(0, 1), Random_Float(0, 1), Random_Float(0, 1), 1) -- just to distinguish them
	Root:Attach(box)
	box.pos:Attach(Create("Draggable", ""))
end

--[[Root:Attach(Create("TestBox", "TestBoxInst"))
TestBoxInst.pos:Attach(Create("Dragable", ""))]]

DebugPrintComponentTree()
