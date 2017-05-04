enemy_pathfinding = {}

-- update function
enemy_pathfinding.update = function(logicComponent, dt, params)
	if params ~= nil then
		if params["movingForward"] == nil then	
			params["movingForward"] = true;
		end
		local angle = logicComponent:getOrientation()
		local pos = logicComponent:getPosition()
		local endPoint = params["end_point"]
		local startPoint = params["start_point"]
		local movingForward = params["movingForward"]
		local speed = 1.0
		if params["move_speed"] ~= nil then
			speed = params["move_speed"]
		end
		if movingForward then
			local dist = logicComponent:getDistance(endPoint, pos)
			if dist < 0.5 then
				local rotAxis = engine.vec3()
				rotAxis.y = 1
				angle = logicComponent:quatMultiply(angle,logicComponent:angleAxis(180,rotAxis))
				params["movingForward"] = false
			end
			local diff = logicComponent:vec3Subtract(endPoint, pos)
			diff = logicComponent:vec3Normalise(diff)
			pos.x = pos.x + (diff.x * dt * speed)
			pos.y = pos.y + (diff.y * dt * speed)
			pos.z = pos.z + (diff.z * dt * speed)
		else
			local dist = logicComponent:getDistance(startPoint, pos)
			if dist < 0.5 then
				local rotAxis = engine.vec3()
				rotAxis.y = 1
				angle = logicComponent:quatMultiply(angle,logicComponent:angleAxis(180,rotAxis))
				params["movingForward"] = true
			end
			local diff = logicComponent:vec3Subtract(startPoint, pos)
			diff = logicComponent:vec3Normalise(diff)
			pos.x = pos.x + (diff.x * dt * speed)
			pos.y = pos.y + (diff.y * dt * speed)
			pos.z = pos.z + (diff.z * dt * speed)
		end
		logicComponent:applyTransform(pos, 1.0, angle);
	else
		print("enemy params nil")
	end
end
-- process message function
enemy_pathfinding.RecieveMessage = function (logicComponent, msg, msgType, params)

end

