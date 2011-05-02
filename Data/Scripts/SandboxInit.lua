Attach(GetObject("RootGameObject"), Create("SandboxProto", "SandboxInst"))
Attach(GetParent(GetObject("Mouse cursor")), GetObject("Magic square"))

Attach(GetParent(GetObject("Mouse cursor")), Create("A", ""))
--Attach(GetParent(GetObject("Mouse cursor")), Create("Poly", ""))
--SetColor(GetObject("TestText"), 1, 0, 0, 1)
--DebugPrintComponentTree()
