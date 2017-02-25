 ----------------------------------------------------------------------------
 SMS Plus for Sega Dreamcast
 ----------------------------------------------------------------------------

 Original code by
 Charles MacDonald

 Ported to Dreamcast by
 Pascal Bosquet & Heliophobe
 
 Introduction
 ------------

 SMS Plus is a Sega MasterSystem/GameGear emulator for the Sega Dreamcast.
  
 Sms Plus for DC is freeware but i'll be pleased having a postal card from you
 if you like it , thanks :)
 my street address is :
 Pascal Bosquet
 134 rue du fort
 4621 LIEGE (BELGIUM)
 
 
 What's New
 ----------

 [Version 0.2DDD]
 - use of DMA texture transfert rather than stored queues
 - added single game emulation
 - added paddle emulation 
 - added preliminary ligth gun emulation
 - added favourites games saving (10 slots)
 - added CodeMaster game mapper
 - added Rapid fire
 - fixed 2 player in-game menu
 - Changed the way save states are saved.Should not break previous states
 - corrected the 32 chars lenght games name mixed up
 - Rewrited vmu handling, unfortunatly now imcompatible with previous version
 - Some others i can remind ;)

 [Version 0.1]

 - Initial release based on 0.9.3
 - No frameskip with sound
 - User friendly gui
 - 2 players support
 - Proper Pal support (tv output and emulation)
 - Add voices emulation
 - Backdrop color painting
 - SRAM and game states saved on VMU
 - Screen adjustment different for sms and gg
 - Bilinear filtering 
 - Great compatibility
 - Snapshot preview in the gui (256 cols pcx)


 Burning instructions
 --------------------
 
 The emulator is distributed as individuals files,the zip file have all the 
 separated files that you'll have to burned for making work the emulator. 
 the cd layout is the same as in the zipped file.

	ip.bin		(start program)
	1st_read.bin	(the program scrambled)
	readme.txt	(this file)
	/snap/sms/	(put your sms snapshot there in 256 colors pcx format)
	/snap/gg/       (put your gg snapshot there in 256 colors pcx format)
	
 Additionally, you can put your ROMS (unzipped !) on this CD.
 This will prevent the cd swap. you can create as much directories as you want.
 Me, i used a per letters ordering which is easier for browsing,it's up to like
 
 For getting games snapshot working :
 Sms : 	Put your snapshots (pcx 256 colors 256x192) in the /snap/sms dir.
 	The pcx must have the same name as the rom, so if your game is named
 	"Alex.sms", you must have a "Alex.pcx" in the /snap/sms/
 Gg :	Put your snapshots (pcx 256 colors 160x144) in the /snap/gg dir.
 	It's the same requirement as the sms, you must have the same name.
 	So with "Ninja.gg", you must have a "Ninja.pcx" in the /snap/gg/

 The directory "snap" won't appear in the gui so it's a reserved name :)

 I advice you to rename all your games using smschecker from Maxim  
 get it at http://www.smspower.org/maxim/

 There's a bunch of site having load of docs, for helping you burning the cd
 selfboot or with cd boot.
 
 www.boob.co.uk (uk)
 www.dcemulation.com (uk)
 www.dcemu.co.uk (uk)
 www.consolevision.com (uk)
 http://dcreload.free.fr (fr)

 I'm not affiliated with those sites in anyway so i cannot be held responsible
 for what the pages may contain. 
 
 Hey! How does it work ?
 -----------------------
 
 The emulator has 5 screens : 
 - menu
 - games select
 - settings
 - credits
 - emulator
 
 - Menu :
 From the menu, you can go to settings , credits and game selector. Just highlight 
 the right icons and press (A)
 
 - Settings : 
 There you can change all the options for the emulator.
 	1) Country (Overseas = Usa , Domestic = Japan )
 	2) MasterSystem tv type : Pal or Ntsc 
 		If you choose Pal , the emulator will output real Pal
 		tv signal , so be sure that your tv support it.
 		It's convenient for european users, who will find back
 		the slow 50hz and the two big bands on the screen :)
 	3) Paint backdrop : Paint the tv backdrop just as the sms would do
 	4) Inverted Buttons : Invert buttons 1 and 2
 	5) Sprites flickering : once enable , sprites will flicker as on the same as
 	   the real hardware.
 	6) Bilinear filtering : enable or not the edge smoothing 
 	7) 8) Sram slot and State slot : you can have up to 10 sram or state slot per
 	      game.
	9) Input Sensitivity : Adjust paddle and light gun sensitivity
	10) Save SRAM : Save the ingame sram to vmu when quitting a game, this
	    useful to disable when you decided to play only with save states
 	11) 12) Adjust the screen for sms and gg.

 Press (B) to get back to menu.
 
 Configuration is saved to vmu (3 blocks), so don't remove the vmu when u quit the
 settings screen
 
 - Credits :
 Display the credits of the emulator.Press (B) to get back to menu.
 
 - Games select :
 There, you can browse the cd. buttons are :
 (A)				load and start the game
 (B) 				parent directory
 (X)				get back to menu
 (Y)				popup favourites screen
 (Start) or (L)+(R)		rejoin a already started game
 Digital Left/Right page Up/Page down
 Digital Up/Down    move selection 
 
 - Emulator:
 Once you have loaded a game , the emulator screen start.
 the controls are as follow :
 
 (A) 				button 1
 (B)				button 2
 
 When rapid fire is off:
	(X)			button 1 + button 2
 When rapid fire is on:	
	(X)			button 1 rapid fired
	(Y)			button 2 rapid fired
	
 (Start)			Pause button
 (L)+(R)			get back to game selection
 (L)+Start or (R)+Start		display the in-emulator menu
 				On this menu you can :
 					- Load/Save a game state
 					- Reset the emulator
 					- Set rapid fire level (0=off 9=fastest)
 					- Get back to games select	

 You can reset the DC on any screen using the well knowned (A)+(B)+(X)+(Y) + (Start)

 - how to use the single game emulation -
 The emulator can auto boot one game directly, so u can do a single game cd.
 Just put your the rom named boot.sms or boot.gg in the cd root and the emulator
 will auto load it
	 		

 Thanks
 ------
 
 Special thanks goes to :
 - Martin Konrad to allow the distribution with gg turrican
 - Brian Peek from Ganksoft(www.ganksoft.com) for helping me a lot and
   being a nice guy :)
 - Christian Nowak (http://chn.roarvgm.com) for his kind help and nice chat
 - Gentil Graphiste (www.bouledefeu.com) for doing this amazing gui design
   and supporting all the cuts i did in his design, sorry pal ;)
 - Charles Mac Donald for doing such a amazing emulator.
 - Dan Potter for the incredible KOS and making this project come true.
   don't forget to buy Feet of Fury (http://www.cagames.com) ;)
 - Takayama Fumihiko (http://nemesis.kaz.ac/%7Efumihit/nesterdc/)
   for letting me borrow the vmu code from Nester DC.
 - and my pals from Spoutnick Team and my little sweetheart Renazen :)

 * PSG by Tatsuyuki Satoh
 * Z80 emulator  Copyright (C) 1998,1999,2000 Juergen Buchmueller

 Contact
 -------

 If you've troubles with this software, be sure before sending us a mail
 to read carefully the whole file. Else you can reach us at :

 Charles Mac Donald (Original emu)
 Mail : cgfm2@hotmail.com
 WWW  : http://cgfm2.emuviews.com

 Pascal Bosquet (Dreamcast port)
 Mail : pastorama@hotmail.com
 WWW  : http://www.pascalorama.com

 Please be aware the WE CAN NOT AND WILL NOT provide ANY
 information on obtaining ROMs .

 You should only be using ROMs that you are legally entitled to have in your
 posession.


 Legal:
 ------

 THIS PROGRAM IS NOT ENDORSED BY, LICENSED BY, WARRANTED BY, OR IN ANY OTHER
 WAY CONNECTED WITH SEGA ENTERPRISES.

 Sega,Dreamcast,MasterSystem,GameGear are registered trademarks
 of Sega Enterprises, Ltd.

 All other trademarks are the property of their respective owners.

 SmsPlus for Dreamcast is freeware, and you can distribute it freely, so long 
 as it is not modified, no copyrighted ROMS are packaged with the program,
 and nothing is charged for distribution except for the media.

 You use this software at your OWN risk, authors are not responsible for any
 loss or damage resulting from the use of this software.
 If you do not agree with these terms do not use this software !