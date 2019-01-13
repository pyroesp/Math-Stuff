# Math-Stuff

Inspired by simuleios on twitch or Leios on github, check him out.

## Bezier Curve:
-------------

Drawing the Bezier Curve the weird way?
Imagine a 3 point bezier ABC/ ABC are locked in place. 
You have an additional point going from A to B, lets call it D, and another one from B to C, lets call it E.
From D to E there's yet another point, lets call it F.
Point F will follow the line DE from start to finish, but because point D and E move along the lines AB and BC, point F will draw a curve.
This is literally what's done in the C code.

![Alt Text](https://media.giphy.com/media/9Pi3Ad2EyzF3JKVcSa/giphy.gif)
