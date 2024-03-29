-- Rotating squares, with relative positioning
---[[
for i = 0, 3 do
	local a = Create("A", "")
	a.pos:SetPosition(0, 0)
	local nextA = a
	while nextA do
		nextA.pos:SetAngle( 90 * i )
		nextA = nextA.A
	end	
	GetObject("Mouse cursor").pos:Attach(a)
end
--]]

-- Single BlackNinja
---[[
local ninja = Create("BlackNinja", "")
ninja.pos:SetPosition(320, 240)
Root:Attach(ninja)
--]]

-- Many arranged BlackNinjas
--[[
for i = 0, 10 do
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
end
--]]


-- Another Ninja
--[[
local another_ninja = Create("BlackNinja", "")
another_ninja.pos:SetPosition(350, 100)
Root:Attach(another_ninja)

-- Cloned Ninja

local cloned_ninja = another_ninja:CloneTree()
cloned_ninja:FindFirstOfClass("PlaceableComponent"):SetPosition(250, 100)
Root:Attach(cloned_ninja)

--]]



-- Some shit, I don't recall what is it
--[[
Root:Attach(Create("SandboxProto", "SandboxInst")
Attach(GetParent(GetObject("Mouse cursor")), GetObject("Magic square"))
--]]

-- Draggable TestBoxes
--[[
for i = 0, 10 do
	local box = Create("TestBox", "")
	box.pos:SetPosition(i * 40, 200)
	box.model:SetColor(Random_Float(0, 1), Random_Float(0, 1), Random_Float(0, 1), 1) -- just to distinguish them
	Root:Attach(box)
	box.pos:Attach(Create("Draggable", ""))
end
--]]

-- Single Draggable TestBox
--[[
Root:Attach(Create("TestBox", "TestBoxInst"))
TestBoxInst.pos:Attach(Create("Dragable", ""))
--]]

-- Clone test
--[[
local text1 = Create("Text", "")
local text2 = text1:Clone()

text1:SetText("Test1")
text2:SetText("Test2")

Create("PlaceableComponent", "Place1")
Place1:Attach(text1)
Place1:SetPosition(100, 200)

local place2 = Place1:Clone()

place2:Attach(text2)

place2:SetPosition(200, 200)

Root:Attach(Place1)
Root:Attach(place2)
--]]

-- Lua-sent events test

local es = Create("EventSender", "")
local er = Create("EventReceiver", "")
Root:Attach(es)
Root:Attach(er)

-- LuaPerfCounter - a text that shows how much memory lua is using
--[[
local lpc = Create("LuaPerfCounter", "")
lpc.pos:SetPosition(10, 300)
Root:Attach(lpc)
--]]

local fonts = { 'DefaultFont', 'iich', 'iich2', 'Font', 'hge', }

for i = 1,5 do
	-- ����� �����, ���� �� �������� ��� ������ ������,
	-- �� ����� �� �������� � �� ����������.
	-- � �� �������� ����. ���� ����� ��� �����.
	local pos = Create("PlaceableComponent", "")
	local text = Create("Text", "")
	pos:Attach( text )
	Root:Attach( pos )
	text:SetText(
[[
jackdaws love my big sphinx of quartz.
JACKDAWS LOVE MY BIG SPHINX OF QUARTZ!?,..
����� �� ��� ���� ������ ����� �� ����� ���.
����� �� �٨ ���� ������ ����� �� ����� ���.
]]	)
	pos:SetPosition( 10, 10 + 80 * i )
	text:SetTextFont(fonts[ i ])
end

-- Makes LuaPerfCounter text draggable
--lpc:Attach(Create("Draggable", ""))

--DebugPrintComponentTree()
