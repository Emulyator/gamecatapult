_ALERT = Game.print
uselib_table()  -- �e�[�u�����C�u�����ǂݍ���
uselib_string() -- �����񃉃C�u�����ǂݍ���
register 'Scene'

dofile("task.lua")

Game.print("�������X�N���v�g", game)

tasks = TaskRunner()

text = Scene.TextPrinter()

stage = Scene.Stage("room1.x")

chr1 = Scene.Entity("../media/gctp_gun.x")
chr1:enter(stage)

chr2 = Scene.Entity("../media/gctp_base.x")
chr2:enter(stage)

cam = Scene.Camera()
cam:enter(stage)

--qcam = Scene.QuakeLikeCamera(cam)
--qcam:enter(stage):setEnable(true)

game:init()

tasks:add(function(delta)
	repeat
		text:setPos(10,300):print("asasasa")
		coroutine.yield()
	until false
end)

game:run({
	update = function(delta)
		tasks(delta)
	end
})

Game.print('# end')
