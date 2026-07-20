# NFC_cartridge_system
An NFC-based cartridge system for physically storing commands, for example to launch games.

# THE CODE FOUND IN THIS REPOSITORY WAS FABRICATED BY AI
Just a warning, I take no credit in the making of the code you can download from here. I personally can't code as well as I'd need to make projects like these, but I try to use such projects as a way to learn how to code myself. I used 2 free LLMs to write the scripts for this project. If you feel like you can write better code for this, feel free to contact me in any way. I will gladly add it to this repo and give you full credit, because despite the fact that I use AI, I still think that human made anything is better by a long shot. 

## HARDWARE:
Arduino Pro Micro x1

NFC Module V3 Elechouse x1

Standard sized LED x1

Standard (20 x 6 x 15mm) limit switch (no lever) x1

A few wires (preferably female jumper wires)

NFC stickers (25mm C) at least one

Plastic filament

3x10mm bolts (can be replaced with 3D printed pegs 3x13mm) x4

Hot glue sticks (optional)

## TOOLS:
3D printer (required)

Hot glue gun (optional)

Screwdriver (optional)

Sandpaper (optional)

2D printer (optional)

Permanent marker (optional)

## SOFTWARE:
Python 3 or newer

Arduino IDE or alternative

Linux (systemd)

Windows (10+)

## How to install PC client
### Windows:
Navigate to the `windows_daemon` directory and run the `install.bat` script. Once that's done, open the newly installed Microsoft PowerToys app at least once and run the install script one more time to apply the correct config.

### Linux:
Navigate to the `linux_daemon` directory and run the `install.sh` script.

## How to create a cartridge:
Install NFC Tools or other such app that allows you to save plain text data to an NFC tag. Create a text record containing the command you want the tag to execute, and write the data to the tag.


### Note:
I do not plan to update this regularly. I allow anyone to update, remake, or repost this as long as you give proper credit. Even if you don't, I won't try to do anything about it.
