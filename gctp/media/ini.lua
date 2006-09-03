_ALERT = Game.print
uselib_table()  -- テーブルライブラリ読み込み
uselib_string() -- 文字列ライブラリ読み込み
register 'Scene'

dofile("task.lua")

Game.print("初期化スクリプト", game)

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
