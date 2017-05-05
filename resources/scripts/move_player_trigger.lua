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
		end
	else
		print("logic or params nil")
	end
end