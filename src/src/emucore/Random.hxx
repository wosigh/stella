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
// $Id: Random.hxx 1724 2009-05-13 13:55:40Z stephena $
//============================================================================

#ifndef RANDOM_HXX
#define RANDOM_HXX

class OSystem;

#include "bspf.hxx"

/**
  This is a quick-and-dirty random number generator.  It is based on 
  information in Chapter 7 of "Numerical Recipes in C".  It's a simple 
  linear congruential generator.

  @author  Bradford W. Mott
  @version $Id: Random.hxx 1724 2009-05-13 13:55:40Z stephena $
*/
class Random
{
  public:
    /**
      Create a new random number generator
    */
    Random();
    
  public:
    /**
      Answer the next random number from the random number generator

      @return A random number
    */
    uInt32 next();

    /**
      Class method which sets the OSystem in use; the constructor will
      use this to reseed the random number generator every time a new
      instance is created

      @param system  The system currently in use
    */
    static void setSystem(const OSystem* system) { ourSystem = system; }

  private:
    // Indicates the next random number
    uInt32 myValue;

  private:
    // Set the OSystem we're using
    static const OSystem* ourSystem;
};

#endif
