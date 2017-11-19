# Reassembly AI Mod Example


How to use
----------

1. Get https://github.com/Akaito/reassembly-ai-mod-example in any one of the following ways, putting the files anywhere convenient.
	- Zip download: https://github.com/Akaito/reassembly-ai-mod-example/archive/master.zip
	- HTTPS git clone: git clone https://github.com/Akaito/reassembly-ai-mod-example.git
	- SSH git clone: git clone git@github.com:Akaito/reassembly-ai-mod-example.git
	- Better yet, fork first, then clone.
2. Open "Reassembly Custom AI 0.sln" in Visual Studio 2017 (free version works fine).
3. If Visual Studio prompts you to install needed features for building C++ desktop programs, backup your settings and installed plugin choices and their settings, then allow the install.
	Be warned it can be a large download.  The backups were because if that install gets interrupted for any reason, you may need to repair VS, which wipes all that data completely.
	- If that gets interrupted or you cancel the install, Visual Studio may stop working completely.  If that happens, run "Visual Studio Installer", then near your installed version, click "More", then "Repair" and let that work for a while.
4. Build the 32-bit Release configuration of the AI mod.
?. ((lua file editing; should we give a template in the AI mod repo?))


Files / directories
-------------------

- game:
	Required headers from Reassembly's code.
- [glm](https://glm.g-truc.net/0.9.8/index.html):
	OpenGL Mathematics library code.


"It's broken"/misbehaving
-------------------------

- Are you accidentally subscribed to your own mod in Steam?
	If you are, you'll have two copies of your mod "installed".
	One in your user files, one in a numbered steamapps directory.
	While it's a good idea to subscribe to your mod from another computer
	to test it, be sure to unsubscribe again on any development machine.

- Old AI DLLs keep reappearing after builds?
	After you rename a project, you'll have to go manually delete the DLLs that
	Visual Studio built for you.  "Build | Clean" won't delete files if VS doesn't
	create them (which, after a rename, it doesn't anymore).

