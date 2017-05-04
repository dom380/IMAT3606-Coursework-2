gold_collectable = {};
gold_collectable.rotationSpeed = 10.0;
gold_collectable.angle = 0.0;

-- returns length of the vec3
gold_collectable.length = function (vec)
	return math.sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
end

-- update function
gold_collectable.update = function(logicComponent, dt, params)
	gold_collectable.angle = gold_collectable.angle + (gold_collectable.rotationSpeed * dt);
	if gold_collectable.angle > 360 then
		gold_collectable.angle = 0
	end
	local pos = engine.vec3();
	pos.x = 0.0;
	pos.y = 0.0;
	pos.z = 0.0;
	local scale = 1.0;
	local rotAxis = engine.vec3()
	rotAxis.y = 1
	local rotation = logicComponent:angleAxis(gold_collectable.angle, rotAxis)
	logicComponent:applyTransform(pos, scale, rotation);
end

gold_collectable.trigger = function(gameObject, engineObj, logic, params)
	local tag = gameObject:getTag()
	if logic then
		if tag == "player" or tag == "PLAYER" then
			print("Gold collected");
			logic:toggleRender();
			logic:updateScore(1);
		end
	end
end

-- process message function
gold_collectable.RecieveMessage = function (logicComponent, msg, msgType, params)

end

