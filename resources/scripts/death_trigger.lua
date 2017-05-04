death_trigger = {}

death_trigger.trigger = function(gameObject, engineObject, param)
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
			if hp == 0 then
				engineObject:replaceScreen("GameOver")
			end
		end
	end
end --function