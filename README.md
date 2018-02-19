# Reassembly AI Mod Example


How to use
----------

1. Fork https://github.com/Akaito/reassembly-ai-mod-example on GitHub.
	Rename the repo after, too, if you don't want it to be "reassembly-ai-mod-example".
2. Clone your forked repo to your computer.
3. Open "AiModExample.sln" in Visual Studio 2017 (free version works fine).
4. If Visual Studio prompts you to install needed features for building C++ desktop programs, see "Visual Studio: building C++ desktop programs steps" below.
5. If this isn't your only local (non-relocated) faction mod, see the steps for "Changing faction ID" below.
	If you don't know what that relates to, you probably don't need to do that.
6. Rename the "VanillaAiMod" project in Visual Studio.
	Use a filesystem-friendly name, as this affects output directories.
7. Build the 32-bit (x86) Release configuration of the AI mod.
	You'll find that it outputs mod files in `%USERPROFILE%/Saved Games/Reassembly/mods/`, in a folder named after the Visual Studio Solution.
8. Run Reassembly.  You should see an "AiModExample" (or your solution name) mod in the "Mods" settings.
9. In the sandbox, use "fleet 80" (or your faction ID, if you changed it) to try spawning your ship.
10. While your ship is selected, use "debug ai" (a toggle) to see custom AIAction names on the ship, to confirm that the AI mod is active.
11. When you make any changes and rebuild your AI DLL, relaunch Reassembly to get it reloaded.

### Visual Studio: "building C++ desktop programs" steps.
1. Backup your settings and installed plugin choices and their settings, then allow the install.
	Be warned it can be a large download.  The backups were because if that install gets interrupted for any reason, you may need to repair VS, which wipes all that data completely.
	- If that gets interrupted or you cancel the install, Visual Studio may stop working completely.  If that happens, run "Visual Studio Installer", then near your installed version, click "More", then "Repair" and let that work for a while.

### Changing faction ID
Change the faction ID from 80 to **the same** other unique value in all of the following areas.
All of these files can be found in `<mod-ai-repo>/mod-assets/`.
If you already built the solution, it copied the files to `%USERPROFILE%/Saved Games/Reassembly/mods/<your-ai-mod-name>/`.
Building tries to be very non-aggressive about overwriting files.  So if you already built the mod once, just go
delete the files from that USERPROFILE directory, then *rebuild* the solution.  *Before* deleting those files,
however, be sure you aren't losing any work if you did any work directly on those outputs.
- factions.lua
	- Line 5: Faction ID 80.
	- Line 28: Faction ID at start of ship file name.
- regions.lua
	- Line 3: ident
	- Line 5: faction
	- Line 9: fleets
- ships/*.lua
	- Change leading "80" in filenames to your new faction ID.
	- Line 3: faction (in each ship file).


Files / directories
-------------------

- mod-assets:
	Faction definition(s) (factions.lua), ships, AI selection, etc.
	((TODO: Add preview.png))
- Anisoptera/Reassembly headers:
	- core: similar to [outlaws-core](https://github.com/manylegged/outlaws-core) library.
	- game: Reassembly-specific headers.
- Non-Anisoptera/-Reassembly headers:
	- [chipmunk](http://chipmunk-physics.net/): Chipmunk open-source physics library.
	- [glm](https://glm.g-truc.net/0.9.8/index.html): OpenGL Mathematics lib code.
	- [re2](https://github.com/google/re2): Regular expression headers.
	- vendor/[SDL2](https://www.libsdl.org/): Simple Directmedia Layer 2 headers.


"It's broken" / misbehaving
-------------------------

- Are you accidentally subscribed to your own mod in Steam?
	If you are, you'll have two copies of your mod "installed".
	One in your user files, one in a numbered steamapps directory.
	While it's a good idea to subscribe to your mod from another computer
	to test it, be sure to unsubscribe again on any development machine.

- Old AI DLLs keep reappearing after builds?
	After you rename a project, you'll have to go manually delete the DLLs that
	Visual Studio built for you.  They're in the "Release" folder.
	"Build | Clean" won't delete files if VS doesn't create them (which, after
	a rename, it doesn't anymore).

