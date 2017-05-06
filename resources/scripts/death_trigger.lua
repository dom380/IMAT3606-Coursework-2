death_trigger = {}

death_trigger.trigger = function(gameObject, engineObj, logic, params)
	local logic = gameObject:getLogic()
	local tag = gameObject:getTag()
	if logic then
		logic:resetTransform()
		if tag == "player" or tag == "PLAYER" then
			local hp = 0
			if params ~= nil and params["string_id"] ~= nil then
				logic:updateScore(-1, params["string_id"]);
				hp = logic:getUIValueInt(params["string_id"])
			else
				logic:updateScore(-1, "heartcount")
				hp = logic:getUIValueInt("heartcount")
			end
			local sound = engine.Sound:getInstance()
			if sound ~= nil then
				sound:play(3)
			end
			if params ~= nil then
				local worldFront = params["world_front"]
				--Todo - Add when Ben's controller change is checked in.
				local worldFrontNegative = params["world_front_negative"]
				if worldFrontNegative == nil then 
					worldFrontNegative = false
				end
				local controller = gameObject:getController()
				if controller and worldFront ~= nil then
					controller:setWorldFront(worldFront.x, worldFront.y, worldFront.z, worldFrontNegative)
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
			if hp == 0 then
				engineObj:replaceScreen("GameOver")
			end
		end
	end
end --function