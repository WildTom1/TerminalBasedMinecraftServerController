# TerminalBasedMinecraftServerController
A terminal based controller for a minecraft server

This is a Linux program that is intended to make managing a Minecraft server more easy.
It provides ways to start, stop, reset the server, aswell as make, delete, and restore backups, and kick all players.

# Requirements
The screen program
A minecraft server

# Config
The config is kept in the /home/USER/.config/serverControl.cfg
The config provides the paths and some varables for the program
The possible items are: screenName, serverPath, backupPath, playerdataBackupPath, and amountOfRam.
The way the file is formated is:

ITEM NAME = VALUE

# Planned Features

An installer

Ability to choose which backup to restore
