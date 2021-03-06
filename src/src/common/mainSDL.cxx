//============================================================================
//
//   SSSS    tt          lll  lll
//  SS  SS   tt           ll   ll
//  SS     tttttt  eeee   ll   ll   aaaa
//   SSSS    tt   ee  ee  ll   ll      aa
//      SS   tt   eeeeee  ll   ll   aaaaa  --  "An Atari 2600 VCS Emulator"
//  SS  SS   tt   ee      ll   ll  aa  aa
//   SSSS     ttt  eeeee llll llll  aaaaa
//
// Copyright (c) 1995-2009 by Bradford W. Mott and the Stella team
//
// See the file "license" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id: mainSDL.cxx 1775 2009-06-17 17:04:55Z stephena $
//============================================================================

#include <SDL.h>
#include <cstdlib>

#include "bspf.hxx"
#include "Console.hxx"
#include "Event.hxx"
#include "EventHandler.hxx"
#include "FrameBuffer.hxx"
#include "PropsSet.hxx"
#include "Sound.hxx"
#include "Settings.hxx"
#include "FSNode.hxx"
#include "OSystem.hxx"
#include "System.hxx"

#if defined(UNIX)
  #include "SettingsUNIX.hxx"
  #include "OSystemUNIX.hxx"
#elif defined(WIN32)
  #include "SettingsWin32.hxx"
  #include "OSystemWin32.hxx"
#elif defined(MAC_OSX)
  #include "SettingsMACOSX.hxx"
  #include "OSystemMACOSX.hxx"
  extern "C" {
    int stellaMain(int argc, char* argv[]);
  }
#elif defined(GP2X)
  #include "SettingsGP2X.hxx"
  #include "OSystemGP2X.hxx"
#else
  #error Unsupported platform!
#endif

#ifdef DEBUGGER_SUPPORT
  #include "Debugger.hxx"
#endif

#ifdef CHEATCODE_SUPPORT
  #include "CheatManager.hxx"
#endif

// Pointer to the main parent osystem object or the null pointer
OSystem* theOSystem = (OSystem*) NULL;

// Does general Cleanup in case any operation failed (or at end of program)
int Cleanup()
{
  if(theOSystem)
    delete theOSystem;

  if(SDL_WasInit(SDL_INIT_VIDEO) & SDL_INIT_VIDEO)
    SDL_Quit();

  return 0;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if defined(MAC_OSX)
int stellaMain(int argc, char* argv[])
#else
int main(int argc, char* argv[])
#endif
{
  // Create the parent OSystem object and settings
#if defined(UNIX)
  theOSystem = new OSystemUNIX();
  SettingsUNIX settings(theOSystem);
#elif defined(WIN32)
  theOSystem = new OSystemWin32();
  SettingsWin32 settings(theOSystem);
#elif defined(MAC_OSX)
  theOSystem = new OSystemMACOSX();
  SettingsMACOSX settings(theOSystem);
#elif defined(GP2X)
  theOSystem = new OSystemGP2X();
  SettingsGP2X settings(theOSystem);
#else
  #error Unsupported platform!
#endif

  theOSystem->settings().loadConfig();

  // Take care of commandline arguments
  string romfile = theOSystem->settings().loadCommandLine(argc, argv);
  FilesystemNode romnode(romfile);

  // Finally, make sure the settings are valid
  // We do it once here, so the rest of the program can assume valid settings
  theOSystem->settings().validate();

  // Create the full OSystem after the settings, since settings are
  // probably needed for defaults
  if(!theOSystem->create())
  {
    cout << "ERROR: Couldn't create OSystem" << endl;
    return Cleanup();
  }

  // Check to see if the user requested info about a specific ROM,
  // or the list of internal ROMs
  // If so, show the information and immediately exit
  if(theOSystem->settings().getBool("listrominfo"))
  {
    theOSystem->propSet().print();
    return Cleanup();
  }
  else if(theOSystem->settings().getBool("rominfo"))
  {
    if(argc > 1 && romnode.exists())
      cout << theOSystem->getROMInfo(romfile);
    else
      cout << "ERROR: ROM doesn't exist" << endl;

    return Cleanup();
  }
  else if(theOSystem->settings().getBool("help"))
  {
    theOSystem->settings().usage();
    return Cleanup();
  }

  // Request that the SDL window be centered, if possible
  if(theOSystem->settings().getBool("center"))
    putenv((char*)"SDL_VIDEO_CENTERED=1");

#ifdef BSPF_UNIX
  // Nvidia cards under UNIX don't currently support SDL_GL_SWAP_CONTROL
  // So we need to do it with an Nvidia-specific environment variable
  // This also means the setting can only be changed by restarting Stella
  // This functionality should really be integrated into SDL directly
  if(theOSystem->settings().getBool("gl_vsync"))
    putenv((char*)"__GL_SYNC_TO_VBLANK=1");
#endif

  //// Main loop ////
  // First we check if a ROM is specified on the commandline.  If so, and if
  //   the ROM actually exists, use it to create a new console.
  // If not, use the built-in ROM launcher.  In this case, we enter 'launcher'
  //   mode and let the main event loop take care of opening a new console/ROM.
  if(argc == 1 || romfile == "" || !romnode.exists() || romnode.isDirectory())
  {
    if(theOSystem->settings().getBool("uselauncher"))
    {
      if(!theOSystem->createLauncher())
        return Cleanup();
    }
    else
    {
      theOSystem->settings().usage();
      return Cleanup();
    }
  }
  else if(theOSystem->createConsole(romfile))
  {
    if(theOSystem->settings().getBool("takesnapshot"))
    {
      for(int i = 0; i < 30; ++i)  theOSystem->frameBuffer().update();
      theOSystem->eventHandler().takeSnapshot();
      return Cleanup();
    }

#ifdef DEBUGGER_SUPPORT
    // Set up any breakpoint that was on the command line
    // (and remove the key from the settings, so they won't get set again)
    const string& initBreak = theOSystem->settings().getString("break");
    if(initBreak != "")
    {
      Debugger& dbg = theOSystem->debugger();
      int bp = dbg.stringToValue(initBreak);
      dbg.setBreakPoint(bp, true);
      theOSystem->settings().setString("break", "");
    }

    if(theOSystem->settings().getBool("debug"))
      theOSystem->eventHandler().enterDebugMode();
#endif
  }
  else
    return Cleanup();

  // Swallow any spurious events in the queue
  // These are normally caused by joystick/mouse jitter
  SDL_Event event;
  while(SDL_PollEvent(&event)) /* swallow event */ ;

  // Start the main loop, and don't exit until the user issues a QUIT command
  theOSystem->mainLoop();

  // Cleanup time ...
  return Cleanup();
}
