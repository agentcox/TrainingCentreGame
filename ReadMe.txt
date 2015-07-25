Changes:
Hit "P" in game to toggle cursor physics; may cause problems on slower machines
Hit "S" in game to toggle slow-motion; for you matrix fans
Hit "W" in game to check what weapon you're using
Hit "H" in game for Help
Hit "ESC" in game to quit


GRAPHICS
0-99
Weapons Data Sprites (280 x 120)
100-199
Weapons Shell Casing Sprites
200-299
Weapons Travel Sprites
300-399
Weapons Hit Sprites
400-499
Weapon Full Bullet Sprites
800-999
Utility Sprites
1000-Up
Target Sprites

SOUND
0 - 99		Weapons Load Sounds
100 - 199	Weapons Rack Sounds
200 - 299	Weapons Fire Sounds
300 - 399	Weapons Hit Sounds
400 - 499	Weapons ShellCasing Bounce Sounds
500 - 599	Utility Sounds
600 - Up	Target Sounds

WEAPONS CLASSES
I	- PISTOLS
II	- SMGS
III	- SHOTGUNS
IV	- AUTO RIFLES
V	- PRECISION RIFLES

Class Identifiers:
45 x 160

SPRITE DATA FILE FORMAT
ID
BitmapFile
FullWidth
FullHeight
FrameWidth
FrameHeight
NumberOfStates
FramesPerState (Comma Delinated, Must END in Comma)

SOUND DATA FILE FORMAT
ID
SoundFile

BULLET DATA FILE FORMAT
Name
0
Full Bullet Sprite ID
0
0
Hit Sprite Width
Hit Sprite Height
Hit Sprite ID
Hit Sound ID
Shell Casing Sprite ID
Shell Casing Bounce Sound ID

WEAPONS DATA FILE FORMAT
Name
Class
Weapon Picture Sprite ID
Use Shot Effects?
Single Shot?
Per-Shot Delay
Clip Loaded?
Max Rounds
Reload Time
Horizontal Recoil
Vertical Recoil
Gunshot Sound ID
Reload Sound ID
Reload Rack Sound ID
Bullet ID
Eject Round When Fired?
Eject All Rounds When Fired?

SOME CHALLENGES

---I. Quick Target Drill (Pts)
This can be for any number of classes, but may work best for precision rifles.

Targets spawn randomly; there can be four targets at most on the board. They are timed, and the timers are
very short. The timers are random. 

The targets will be scored only upon their timers disappearing, and you will only be scored upon
the smallest ring hit. That is, one "10" is scored as a 10, and two "8"'s is scored as an 8. So the idea is to
sight-acquire fire and keep your eyes on which targets are going to die off the quickest.


---II. Duck Hunt (%)
This can be for any number of classes, but is traditionally done with a shotgun.

Clay pigeons are thrown, and are shot down. There can only be one on the screen at a time.
If the pigeon leaves the opposite side of the screen than it started from and is not in the process of dying, then the
player is given a zero for that pigeon. If, however, it is killed, the player gets a 1. It's for percentage, so the
more you hit, the better your percentage.


---III. Hostage Drill (%)
This can be for any number of classes, EXCEPT shotgun.

Hostage targets are spawned, one on the screen at a time maximum. They are timed. The timers are short, 
and if the "bad guy" is not killed, the hostage will die.

If the hostage is shot, zero points will be awarded. If the timer runs out, zero points will be awarded. The only way to get
a point is to kill the bad guy.


---IV. Target Termination (Pts)
This can be for any number of classes.

One target. HUGE target.
Shoot it, score as many points as possible.
