# Math-Stuff

Inspired by simuleios on twitch or Leios on github, check him out.

## Bezier Curve:

Drawing the Bezier Curve the weird way?<br/>
Imagine a 3 point bezier ABC. The ABC points are locked in place. <br/>
You have an additional point going from A to B, lets call it D, and another one from B to C, lets call it E.<br/>
From D to E there's yet another point, lets call it F.<br/>
Point F will follow the line DE from start to finish, but because point D and E move along the lines AB and BC, point F will draw a curve.<br/>
This is literally what's done in the C code.<br/>

![Alt Text](https://media.giphy.com/media/9Pi3Ad2EyzF3JKVcSa/giphy.gif)

Newer version : https://imgur.com/C0ySh6R

Use left mouse click to add a point and right mouse click to remove.<br/>
When there are 3 points or more, the Bezier Curve starts automatically.<br/>

Special thanks to Deetle from Twitch who helped me fix my mouse button issue during his stream and thought me about a faster way to add and remove elements in dynamic arrays.

- Uses SDL2