# Giroflex VSL (VehicleSirenLights)

<h1>Tested APKs</h1>

* GTASA 2.00 AML 1.2.1 FLA 7.2
* SAMP Mobile Alyn v2
  * NOTE: you must use SAUtils 1.3.1
* SAMP Mobile Alyn v11.0
  * NOTE: you must go to options, and change 'SAMP-Version' to 'Old'
  * you must use SAUtils 1.3.1
* SAMP Mobile Alyn v11.6
  * NOTE: you must go to options, and change 'SAMP-Version' to 'Old'
  * you must use SAUtils 1.3.1
* SAMP Mobile Alyn v13.1
  * NOTE: it's not compatible with SAUtils (only version 1.1)

<h1>Install</h1>

## Required libs (download separatedly)
  * Menu VSL (https://github.com/Danilo1301/GTASA_libMenuVSL)

## Required libs (comes with the .zip)
  * AML (https://github.com/RusJJ/AndroidModLoader)
  * SAUtils (https://github.com/AndroidModLoader/SAUtils)
  * CLEO Mod (https://github.com/AndroidModLoader/GTA_CLEOMod)
  * BASS Mod (https://github.com/AndroidModLoader/BASSMod)

1. Download the **GiroflexVSL-0.0.0__en.zip** file in [Releases](https://github.com/Danilo1301/GTASA_libGiroflex/releases)
2. Open the folder "1. required libs" and copy the "mods" folder to your "mods" folder (in com.rockstargames.gtasa/mods)
3. Open the folder "2. inside game folder", copy the folders "mods" and "configs" and paste it in your game folder (in com.rockstargames.gtasa)
4. Open the folder "3. inside CLEO" and copy the files "GiroflexVSL.csa" and "GiroflexVSL.fxt" to your game folder (in com.rockstargames.gtasa)
5. OPTIONAL: Open the folder "optional lib (for audio playing)" and copy the "mods" folder to your "mods" folder (in com.rockstargames.gtasa/mods)
6. Install the mod textures inside the 'txd.txt' using 'TXD Tool' (Tutorials below)

<h1>How to use</h1>

* Press the 'imaginary buttons' 5 and 6 ([example](https://imgur.com/gA8Lfvt)) for 0.5s to turn lights on/off
* Press the 'imaginary buttons' 5 and 6 ([example](https://imgur.com/gA8Lfvt)) for 1s to open the editing menu
* Panel: press the 'imaginary buttons' 5 and 9 ([example](https://imgur.com/LLddMfg)) for 0.5s

<h1>Tutorials</h1>

* Installing the giroflex mod: https://youtu.be/4mq7lfXUUfY
* Installing the folder "Mute original siren": https://www.youtube.com/watch?v=GdR_3ZIc_6Y

<br>

* Installing the textures of the audio panel: https://youtu.be/ghzIKrxtvx4
* Installing the retangular lights textures: https://youtu.be/A9JWQilIvYI

<br>

* Adding/replacing siren sounds: https://youtu.be/vYm1lYQIGvk
* Installing GTA SA 2.00 with original data: https://youtu.be/wT4N_NAYFOI
* How to create a flash pattern: https://youtu.be/KZkaiiXhtLU
* How to convert .INI files to .JSON: https://youtu.be/p_R810KUDb4

<h1>Tutorials (Adapting)</h1>

* Adapting a vehicle: https://youtu.be/UXQQKfLok-o

<h1>Config</h1>

Each vehicle has a .ini file where it stores its config<br>
Those files are located at: GAME_FOLDER/configs/giroflex

<h1>Issues</h1>

<h3>Crash with the libBASSMod.so</h3>

Some users have reported crashes with the new libBASSMod (which is for playing audio). If your game is crashing, try removing 'libBASSMod.so' from /mods folder

<h3>Message 'Remove the old giroflex.csa and .fxt'</h3>

If you see this message it means that there is a file called 'giroflex.csa' is one of the following folders:

* in the CLEO folder, in: /storage/emulated/0/cleo/sa/giroflex.csa
* or in the game folder, in: data/com.rockstargames.gtasa/giroflex.csa

You only need to delete this file (giroflex.csa) and the giroflex.fxt

<h3>Mods cleo not showing on menu</h3>

You just need to go in the game, then "Options" > "Game" and change the "CLEO LOCATION"<br>

* OLD CLEO - com.rockstargames.gtasa/ mod.csi
* OLD CLEO (+CLEO) - com.rockstargames.gtasa/cleo/ mod.csi
* ../FILES/CLEO - com.rockstargames.gtasa/files/cleo/ mod.csi
* CLEO 2.0.1 - storage/emulated/0/cleo/sa/ mod.csi

(Don't forget to restart the game to apply the changes)

<h3>Menu too much to the left</h3>

To change the menu's position, you need to enable the edit mode in: "Options" > "Mods Settings" > "Giroflex VSL - Edit mode"

<h3>Bent lights / Lights rotated to the side by a little</h3>

This happens when you use libJPatch.so with 'PS2CoronaRotation' enabled. To disable you have to go to your configs, in the .ini file, go the where this option is located and change it to 0 (PS2CoronaRotation=0)

<h1>Discord</h1>

* https://discord.gg/mkCDRf4zJA

<h1>Lib versions</h1>

For all versions:
* BASS Mod: 1.0.0.0
* CLEO Mod: 2.0.1.3
* AML Headers: 1.1

For SAMP:
* SAUtils: 1.3.1
* AML: 1.0.2.2

For GTA SA 2.00
* SAUtils: 1.6
* AML: 1.2.1

For GTA SA 2.10
* Not tested yet

<h1>Credits</h1>

* Mod made using AML (https://github.com/RusJJ/AndroidModLoader)

* Project base used from CLEOMod (https://github.com/AndroidModLoader/GTA_CLEOMod)
  
* Siren icon from: https://www.vecteezy.com/vector-art/5728560-alarm-light-siren-solid-icon-vector-illustration-logo-template-suitable-for-many-purposes

* Config icon from: https://www.flaticon.com/free-icon/config_8053573