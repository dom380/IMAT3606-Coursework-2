trigger_test = {};
trigger_test.foundIds = {};

trigger_test.trigger = function(gameObject)
	local modelComp = gameObject:getModel()
	local id = gameObject:getId()
	local hitBefore = false
	--Check if collided with this object before
	--Works, causes a big frame drop on first collision
	for k,v in pairs(trigger_test.foundIds) do
		if(id == v) then 
			hitBefore = true
			break
		end
	end
	if modelComp ~= nil then --If object has a model and hasn't be collided before, change colour
		if hitBefore ~= true then
			table.insert(trigger_test.foundIds, id)
			local colour = engine.vec3();
			colour.x = 0.0;
			colour.y = 0.0;
			colour.z = 1.0;
			local material = engine.Material();
			material.Kd = colour
			material.Ka = colour
			local msg = engine.MaterialMessage(material) --Construct message
			modelComp:recieveMessage(msg)
		end
	else
		print("Model Component is null");
	end
end
