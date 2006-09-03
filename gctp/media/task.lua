TaskRunner = {}
function TaskRunner:add(f)
	if not self.newcomer then self.newcomer = {} end
	if type(self) ~= 'table' then error("tasks.addでなくtasks:add") end
	table.insert(self.newcomer, coroutine.create(f))
end
function TaskRunner:def(n, f)
	if not self.newcomer then self.newcomer = {} end
	self.newcomer[n] = coroutine.create(f)
end
function TaskRunner:kill(n)
	if not self.grave then self.grave = {} end
	self.grave[n] = true
end
function TaskRunner:clear()
	self.newcomer = nil
	self.do_clear = true
end
function TaskRunner:alive(n)
	return self.running and self.running[n] ~= nil and coroutine.status(self.running[n]) ~= "dead"
		or self.newcomer and self.newcomer[n] ~= nil and coroutine.status(self.newcomer[n]) ~= "dead"
end
function TaskRunner:execute(delta)
	if self.do_clear then
		self.running = nil
		self.grave = nil
		self.do_clear = nil
	end
	if self.grave then
		if self.running then
			for k, v in pairs(self.grave) do
				self.running[v] = nil
			end
		end
		self.grave = nil
	end
	if self.newcomer then
		if not self.running then self.running = {} end
		for k, v in pairs(self.newcomer) do
			if type(k) == 'number' then
				table.insert(self.running, v)
			else
				self.running[k] = v
			end
		end
		self.newcomer = nil
	end
	if self.running then
		local i = 0
		for k, v in pairs(self.running) do
			local ok, msg = coroutine.resume(v, delta)
			if ok then
				i = i + 1
			else
				error("coroutine実行中に以下のエラーを検知 :\n "..msg)
			end
			if coroutine.status(v) == "dead" then
				self:kill(k)
			end
		end
		if i == 0 then self.running = nil end
	end
	if self.do_clear then
		self.running = nil
		self.grave = nil
		self.do_clear = nil
	end
	if self.grave then
		if self.running then
			for k, v in pairs(self.grave) do
				self.running[k] = nil
			end
		end
		self.grave = nil
	end
end
setmetatable(TaskRunner,{
	__call = function(self)
		ret = {}
		setmetatable(ret, {
			__call = function(self, delta)
				TaskRunner.execute(self, delta)
			end,
			__index = function(self, key)
				if TaskRunner[key] then return TaskRunner[key] end
				return rawget(self, key)
			end
		})
		return ret
	end
})
