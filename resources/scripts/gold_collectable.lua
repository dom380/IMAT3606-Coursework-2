gold_collectable = {};
gold_collectable.rotationSpeed = 50.0;

-- returns length of the vec3
gold_collectable.length = function (vec)
	return math.sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
end

-- update function
gold_collectable.update = function(logicComponent, dt)
	local angle = (gold_collectable.rotationSpeed * dt);
	local pos = engine.vec3();
	pos.x = 0.0;
	pos.y = 0.0;
	pos.z = 0.0;
	scale = 1.0;
	logicComponent:applyTransform(pos, scale, angle);
end

-- process message function
gold_collectable.RecieveMessage = function (logicComponent, msg, msgType)
	if msgType == "LOCATION" then
		local dist = engine.vec3();
		local pos = logicComponent:getPosition();
		local location = msg.location;
		dist.x = location.x - pos.x;
		dist.y = 0;
		dist.z = location.z - pos.z;
		distance = math.abs(gold_collectable.length(dist));
		if distance < 8 and logicComponent:isRendering() then
			print("Gold collected");
			logicComponent:toggleRender();
			logicComponent:updateScore(1);
		end
	end
end

