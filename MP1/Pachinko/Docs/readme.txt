** Known Issues ** 
Balls seem to favor the right side of the machine.  Balls "stick" together when speed is low.  I ended up having to filter slow speeds out of the momentum transfer function to get it halfway working.

** How to Use **

-- Controls --
Pressing 'I' adds a ball to the machine, holding Shift adds many balls
Pressing 'O' deletes a ball from the machine, holding Shift deletes many balls
Holding 'T' slows time to 10% speed, holding Shift slows it to 1% speed
Pressing '[' decreases ball size
Pressing ']' increases ball size
Pressing 'K' increases gravity by 10%
Pressing 'J' decreases gravity inversely to the increase amount
Pressing 'M' increases global elasiticity factors by 5%
Pressing 'N' decreases global elasiticity factors by 5%
Pressing 'P' or Pause toggles pause mode
Pressing 'W' toggles liquid rendering on and off (at fixed reddish color to contrast with blue background)

ESC exits game

-- Gameplay -- 
This is a pachinko machine where balls fall down the machine and hit bumpers and other balls on their way down to the bottom.  When the balls reach the bottom, they warp back to the top.

** Deep Learning **

I recycled a lot of Asteroids code to do the physics.  I changed the momentum transfer function from Asteroids to a general function in a new PhysicsUtil.cpp file.  This way I can abstract the function to generic mass, position, velocity and elasticity values and detach it from game-centric code.  This should prove useful in upcoming projects.

The sticky ball issue caused me a lot of trouble when coding and debugging the machine.  I never really fixed it, but came up with a solution in my momentum transfer function that does nothing when total incoming velocity is under a threshold value.  It's a hack that seems to work, but I wish I could get it to work in a general case without the filter.  

I think I should have started the project sooner.  I had to stuff all the work into two days instead of planning it out and having more time to fix bugs.  