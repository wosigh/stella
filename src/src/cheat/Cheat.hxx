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
// Copyright (c) 1995-2008 by Bradford W. Mott and the Stella team
//
// See the file "license" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id: Cheat.hxx,v 1.7 2008/02/06 13:45:19 stephena Exp $
//============================================================================

#ifndef CHEAT_HXX
#define CHEAT_HXX

#include "StringList.hxx"
#include "OSystem.hxx"
#include "bspf.hxx"

class Cheat
{
  public:
    Cheat(OSystem* osystem, const string& name, const string& code)
      : myOSystem(osystem),
        myName(name),
        myCode(code),
        myEnabled(false)
    {
      if(name == "") myName = code;
      myName = StringList::removePattern(myName, "\":");
    }
    virtual ~Cheat() { }

    bool enabled() const { return myEnabled; }
    const string& name() const { return myName; }
    const string& code() const { return myCode; }

    virtual bool enable() = 0;
    virtual bool disable() = 0;

    virtual void evaluate() = 0;

  protected:
    static uInt16 unhex(const string& hex)
    {
      int ret = 0;

      for(unsigned int i=0; i<hex.size(); i++) {
        char c = hex[i];

        ret *= 16;
        if(c >= '0' && c <= '9')
          ret += c - '0';
        else if(c >= 'A' && c <= 'F')
          ret += c - 'A' + 10;
        else
          ret += c - 'a' + 10;
      }
      return ret;
    }

  protected:
    OSystem* myOSystem;

    string myName;
    string myCode;

    bool myEnabled;
};

#endif
