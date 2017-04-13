trigger_test = {};

trigger_test.trigger = function(gameObject)
	local modelComp = gameObject:getModel()
	if modelComp ~= nil then
		local colour = engine.vec3();
		colour.x = 0.0;
		colour.y = 0.0;
		colour.z = 1.0;
		local material = engine.Material();
		material.Kd = colour
		material.Ka = colour
		local msg = engine.MaterialMessage(material) --Construct message
		modelComp:recieveMessage(msg)
	else
		print("Model Component is null");
	end
end
