-- Ian Milligan 
-- Sample test script

gold = gr.material({0.9, 0.8, 0.4}, {0.8, 0.8, 0.4}, 25)
red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 25)
blue = gr.material({0.0, 0.0, 1.0}, {0.5, 0.5, 0.5}, 5)
brown = gr.material({0.4, 0.1647, 0.1647}, {0.1, 0.1, 0.1}, 20)
stone = gr.material({0.4, 0.4, 0.4}, {0.0, 0.0, 0.0}, 0)
sunset_yellow = gr.material({0.8, 0.45, 0.05}, {0.1, 0.1, 0.1}, 25)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)

require('readobj')

-- The parent scene node
scene = gr.node('scene')
scene:translate(0, -3, -12)
scene:rotate('X', 10)

-- Setup the boat that the cow will be sitting on
boat = gr.node('boat')
scene:add_child(boat)
boat:translate(-3, -0.5, -0.5)
boat:scale(2.75, 1.5, 1.25)
boat:rotate('Y', -45)

-- The boat stern (behind)
boat_stern = gr.cube('boat_stern')
boat_stern:set_material(brown)
boat:add_child(boat_stern)

-- The boat bow (front)
boat_bow = gr.mesh('bow', {
	{1, 1, 1},
	{1, 0, 1},
	{1, 0, 0},
	{1, 1, 0},
	{1.75, 1, 0.4}
}, {
	{4, 3, 0},
	{4, 0, 1},
	{4, 1, 2},
	{4, 2, 3}
})
boat:add_child(boat_bow)
boat_bow:set_material(brown)

-- The sail
sail = gr.mesh('sail', {
  {0.21, 2.5, 0},
	{0.21, 1.5, 0},
	{0.21, 1.5, 1},
	{0.21, 2.5, 1}
}, {
  {3, 2, 1, 0}
})
boat:add_child(sail)
sail:set_material(red)
sail:translate(0.2, 0, 0)

-- Mast, represented by a thin box
mast = gr.node('mast')
boat:add_child(mast)
mast:translate(0.2, 1, 0.4)
mast:scale(0.2, 1.5, 0.2)
mast_box = gr.cube('mast_box')
mast:add_child(mast_box)
mast_box:set_material(brown)

-- The COW!
cow_poly = gr.mesh('cow', readobj('cow.obj'))
factor = 2.0/(2.76+3.637)
cow_poly:set_material(gold)
cow_poly:translate(1.25, 1.5, 0.5)
-- Bring the cow into a workable size
cow_poly:scale(factor, factor, factor)
-- Apply our own scaling to it.
cow_poly:scale(1 / 2.75, 1 / 1.5, 1 / 1.25)
cow_poly:scale(0.75, 0.75, 0.75)
boat:add_child(cow_poly)

-- The mountains in the background
mountains = gr.node('mountains')
scene:add_child(mountains)
mountains:scale(2, 1, 1)
mountains:translate(-10, 0, -20)

mountain_number = 1
for _, pt in pairs({
		      {{0, 0, 0}, 5},
          {{1, 0, 0}, 3},
		      {{2, 0, 0.25}, 4},
		      {{3, 0, 0.5}, 5},
          {{4, 0, 0.5}, 2},
          {{5, 0, 0.25}, 3},
		      {{6, 0, 0.125}, 4},
		      {{8, 0, 0}, 7}}) do
   mountain_instance = gr.nh_sphere('mountain' .. tostring(mountain_number), pt[1], 1)
   mountains:add_child(mountain_instance)
   mountain_instance:scale(pt[2], pt[2], 1)
   mountain_instance:set_material(stone)
   mountain_number = mountain_number + 1
end

-- The sunset
sunset = gr.node('sunset')
scene:add_child(sunset)
sun = gr.sphere('sun')
sunset:scale(4, 4, 1)
sunset:translate(0, 0.75, -25)
sunset:add_child(sun)
sun:set_material(sunset_yellow)

-- The buoy
parent_buoy = gr.node('parent_buoy')

ball = gr.sphere('ball')
parent_buoy:add_child(ball)
ball:set_material(white)

pole_1 = gr.cube('pole_1')
parent_buoy:add_child(pole_1)
pole_1:set_material(white)
pole_1:rotate('X', -30)
pole_1:translate(-0.2, 0, 0.9)
pole_1:scale(0.2, 2.15, 0.2)

pole_2 = gr.cube('pole_2')
parent_buoy:add_child(pole_2)
pole_2:set_material(white)
pole_2:rotate('Y', 90)
pole_2:rotate('X', -30)
pole_2:translate(-0.2, 0, 0.9)
pole_2:scale(0.2, 2.15, 0.2)

pole_3 = gr.cube('pole_3')
parent_buoy:add_child(pole_3)
pole_3:set_material(white)
pole_3:rotate('Y', 180)
pole_3:rotate('X', -30)
pole_3:translate(-0.2, 0, 0.9)
pole_3:scale(0.2, 2.15, 0.2)

pole_4 = gr.cube('pole_4')
parent_buoy:add_child(pole_4)
pole_4:set_material(white)
pole_4:rotate('Y', 270)
pole_4:rotate('X', -30)
pole_4:translate(-0.2, 0, 0.9)
pole_4:scale(0.2, 2.15, 0.2)

tip = gr.sphere('tip')
parent_buoy:add_child(tip)
tip:set_material(red)
tip:translate(0, 2.1, 0)
tip:scale(0.4, 0.4, 0.4)


-- Actual implementations of the buoys
buoy_1 = gr.node('buoy_1')
scene:add_child(buoy_1)
buoy_1:add_child(parent_buoy)
buoy_1:translate(2, -0.5, -10)

buoy_2 = gr.node('buoy_2')
scene:add_child(buoy_2)
buoy_2:add_child(parent_buoy)
buoy_2:translate(2, -0.5, -0.5)
buoy_2:rotate('Y', -45)

buoy_3 = gr.node('buoy_3')
scene:add_child(buoy_3)
buoy_3:add_child(parent_buoy)
buoy_3:translate(6, -0.5, -2)
buoy_3:rotate('Y', -15)

-- The ocean
plane = gr.mesh('plane', {
	{-1, 0, -1},
	{ 1, 0, -1},
	{1,  0, 1},
	{-1, 0, 1},
     }, {
	{3, 2, 1, 0}
})
scene:add_child(plane)
plane:set_material(blue)
plane:scale(30, 30, 30)

-- The lights
sun = gr.light({0, 15,-25}, {1.0, 0.9, 0.5}, {1, 0, 0})
camera_light = gr.light({0, 50, 50}, {0.25, 0.25, 0.25}, {1, 0, 0})

gr.render(scene, 'sample.png', 256, 256, 
	  {0, 0, 0,}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.3, 0.3}, {sun, camera_light})
