move_player_trigger = {}

move_player_trigger.trigger = function(gameObject, engineObj, logic, params)
	local tag = gameObject:getTag()
	local logicComp = gameObject:getLogic()
	if logicComp and params ~= nil then
		if tag == "player" or tag == "PLAYER" then
			local posToMove = params["new_pos"]
			if posToMove ~= nil then
				logicComp:applyTransform(posToMove, 1.0, logicComp:getOrientation())
			end
			local worldFront = params["world_front"]
			--Todo - Add when Ben's controller change is checked in.
			-- local worldFrontNegative = params["world_front_negative"]
			-- if worldFrontNegative == nil then 
				-- worldFrontNegative = false
			-- end
			local controller = gameObject:getController()
			if controller and worldFront ~= nil then
				controller:setWorldFront(worldFront.x, worldFront.y, worldFront.z)
			end
			local camDistance = params["camera_distance"]
			if controller and camDistance then
				controller:setCameraDistance(camDistance)
			end
			local camDirection = params["camera_direction"]
			if controller and camDirection then
				controller:setCameraDirection(camDirection)
			end
		end
	else
		print("logic or params nil")
	end
end