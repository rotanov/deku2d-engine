--[[
Attach(GetObject("RootGameObject"), Create("SandboxProto", "SandboxInst"))
Attach(GetParent(GetObject("Mouse cursor")), GetObject("Magic square"))
--]]
local a = Create("A", "")
a.pos:SetPosition(0, 0)
Attach(GetParent(GetObject("Mouse cursor")), a.object)
--Attach(GetParent(GetObject("Mouse cursor")), Create("Poly", ""))
--SetColor(GetObject("TestText"), 1, 0, 0, 1)
--DebugPrintComponentTree()

--[[
	���� ����� �� ����� ���������� �������, � �� ������ ������������.
	������������ � ��� ����� � {}.object
	
]]