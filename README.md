# Platformer FPS

## <h2 id="top">Summary</h2>
---
-[Currently implemented](#implements)   
-[Project UML](#architecture)  
-[Controls](#ctrl)  
-[Changing the scene's assets](#scene)  
-[Additionnal notes](#notes)  
-[External libraries](#libs)  

## <h2 id="implements">Currently implemented</h2>
---
- Spherical player that can move and jump  
- Scene parser  
- Cubic platforms with modifiable textures   
- Collision between the player and the platforms, Enemies with platforms, player, bullet , bullet with enemies, walls and platforms 
- Enemies who's able to chase a player across the map, jump when they are near a Box and can stop when they are next to a cliff
- The player is equiped with a weapon with 5 ammo and he is able to shoot the enemies and reload
- A menu with 3 buttons: Play, Credits and Quit


![V0](ScreenShots/01.gif)  

## <h2 id="architecture">Project UML</h2>
---
![UML](UML/UML_platformer.png)  
Note: this picture of the UML might not be up to date as we modify it everytime the architecture changes, without always exporting and pushing the picture of it. You can find the most recent version [here](https://lucid.app/lucidchart/invitations/accept/inv_7613dabb-0504-42b0-9614-6adee5bc3731?viewport_loc=1232%2C-1009%2C3489%2C1759%2C0_0).  

## <h2 id="ctrl">Controls</h2>
---
- Use WASD (Qwerty) or ZQSD (Azerty) to move the player and the camera will follow  
- Press space to jump
- Pres leftShift to handle sneak
- Press escape to return in the main menu
- Press left click to shoot
- Press r to reload  

## <h2 id="scene">Changing the scene's assets</h2>
---
You can modify the scene by modifying the `level.txt` file in the resources folder. The syntax is detailed in this section.  
In the following instructions, `string` means you must type the argument, such as a state or a file path, and `vec3` is a tridimensional vector, which must follow the syntax `x,y,z` with x, y and z being three numbers. You must specify the number like in a wavefront object (0 or 0.0 but not 0f, 0.0f or 0.f)and separate them with a comma without any space.  
To specify a path for the resources, type the line `path <string:path>`. To create a model, type: `m <string:textureName> <string:meshName> <string:shaderName> <vec3:position>|<vec3:rotation>|<vec3:scale>`. Replace the `m` with a `p` to create the player, with a `w` for the weapon and `pl` to create a platform. Note that for the weapon, the position is relative to the player, therefore it will take its scale and rotation into consideration.   
To modify the camera, type: `c <string:state> <additionnal arguments>`. The states supported by this syntax are `follow`, `fps` and `lock`. `Follow`'s arguments are  `target` and `offset`. The offset is the position (passed as a `vec3`) of the camera relative to its target, and the target is either `p` for targetting the player, either none, in which case the camera won't follow anything until specified in the code. `fps` takes the same parameters and only allows the camera's rotation to be independant to the target's rotation. If the state isn't any of the ones listed above, the default state will be free, and there is no additionnal argument.  The last line about the camera will be the one that defines most if not every part of its behavior.  
Finally, the lights can be modified by entering  `l <int:index> <string:on/off> <vec3:position> <vec3:ambient>|<vec3:diffuse>|<vec3:specular> <vec3:attenuation>`. Index must be under or equal to `MAX_LIGHTS - 1`. On/off defines if the light will be used by the shader.  

## <h2 id="notes">Additionnal notes</h2>
---
There can only be one player in the scene, if there are multiple players in the .txt, only the last player will be created. plus, a scene must have a player.  
Every scene needs a weapon too.  
If you launch the project from Visual Studio, make sure you are in Debug and x64. If there are still problems, check your version, as it might not work on versions older than VS2019 (Version 16.0.30225 D16.6).  
Loading objects can be a long process and the console currently doesn't display any information about the loading, except if there is an error.  
If there is a problem with the first loaded shader (for example, the .vert and the .frag have different names/paths), the console might be flooded with the same error and other shaders might not work if they require uniforms.  
Collision in the angle of the object doesn't work, you cross the object and you will be stuck in it.  
All resources are on Sirius.  

## <h2 id="libs">External libraries</h2>
---
**This project uses:**  
-GLFW   
-stbimage  

[Top of the page](#top)