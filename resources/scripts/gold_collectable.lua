rotationSpeed = 50.0;

-- returns length of the vec3
function length(vec)
	return math.sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
end

-- update function
function update (logicComponent, dt)
	angle = (rotationSpeed * dt);
	pos = engine.vec3();
	pos.x = 0.0;
	pos.y = 0.0;
	pos.z = 0.0;
	scale = 1.0;
	logicComponent:applyTransform(pos, scale, angle);
end

function RecieveMessage(logicComponent, msg, msgType)
	if msgType == "LOCATION" then
		local dist = engine.vec3();
		pos = logicComponent:getPosition();
		location = msg.location;
		dist.x = location.x - pos.x;
		dist.y = 0;
		dist.z = location.z - pos.z;
		local distance = math.abs(length(dist));
		if distance < 8 and logicComponent:isRendering() then
			print("Gold collected");
			logicComponent:toggleRender();
			logicComponent:updateScore(1);
		end
	end
end

