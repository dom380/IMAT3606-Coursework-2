collectable = {};
collectable.rotationSpeed = 10.0;
collectable.angle = 0.0;

-- returns length of the vec3
collectable.length = function (vec)
	return math.sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
end

-- update function
collectable.update = function(logicComponent, dt, params)
	collectable.angle = collectable.angle + (collectable.rotationSpeed * dt);
	if collectable.angle > 360 then
		collectable.angle = 0
	end
	local pos = engine.vec3();
	pos.x = 0.0;
	pos.y = 0.0;
	pos.z = 0.0;
	local scale = 1.0;
	local rotAxis = engine.vec3()
	rotAxis.y = 1
	local rotation = logicComponent:angleAxis(collectable.angle, rotAxis)
	logicComponent:applyTransform(pos, scale, rotation);
end

collectable.trigger = function(gameObject, engineObj, logic, params)
	local tag = gameObject:getTag()
	if logic then
		if tag == "player" or tag == "PLAYER" then
			logic:toggleRender();
			if params ~= nil and params["string_id"] ~= nil then
				logic:updateScore(1, params["string_id"]);
			else
				logic:updateScore(1, "score_string");
			end
		end
	end
end

-- process message function
collectable.RecieveMessage = function (logicComponent, msg, msgType, params)

end

