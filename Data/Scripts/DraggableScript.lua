DraggableComponent = GameObject:Derive()

function DraggableComponent:OnCreate()
	self.Drag = false
end

function DraggableComponent:OnAttached()
	self:Subscribe("MouseDown")
	self:Subscribe("MouseUp")
	self:Subscribe("MouseMove")
end

function DraggableComponent:OnDetached()
	self:Unsubscribe("MouseDown")
	self:Unsubscribe("MouseUp")
	self:Unsubscribe("MouseMove")
	self.Drag = false
end

function DraggableComponent:OnMouseDown(e)
	local ControlledObject = self.proto:FindFirstOfClass("PlaceableComponent")

	if not ControlledObject then
		return
	end

	local box = ControlledObject:GetBox()

	local x = e.X
	local y = e.Y

	if x > box.Min.x and x < box.Max.x and y > box.Min.y and y < box.Max.y then
		self.Drag = true
	end
end

function DraggableComponent:OnMouseUp()
	self.Drag = false
end

function DraggableComponent:OnMouseMove(e)
	local ControlledObject = self.proto:FindFirstOfClass("PlaceableComponent")

	if not ControlledObject or not self.Drag then
		return
	end

	local x = e.X
	local y = e.Y

	ControlledObject:SetPosition(x, y);
end
