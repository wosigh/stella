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
// $Id: Cart.hxx 1849 2009-08-05 16:05:34Z stephena $
//============================================================================

#ifndef CARTRIDGE_HXX
#define CARTRIDGE_HXX

#include <fstream>
#include <sstream>

class Cartridge;
class System;
class Properties;
class Settings;

#include "bspf.hxx"
#include "Array.hxx"
#include "Device.hxx"

/**
  A cartridge is a device which contains the machine code for a 
  game and handles any bankswitching performed by the cartridge.
 
  @author  Bradford W. Mott
  @version $Id: Cart.hxx 1849 2009-08-05 16:05:34Z stephena $
*/
class Cartridge : public Device
{
  public:
    /**
      Create a new cartridge object allocated on the heap.  The
      type of cartridge created depends on the properties object.

      @param image    A pointer to the ROM image
      @param size     The size of the ROM image 
      @param md5      The md5sum for the given ROM image (can be updated)
      @param name     The name of the ROM (can be updated)
      @param type     The bankswitch type of the ROM image
      @param settings The settings associated with the system
      @return   Pointer to the new cartridge object allocated on the heap
    */
    static Cartridge* create(const uInt8* image, uInt32 size, string& md5,
                             string& name, string type, Settings& settings);

    /**
      Create a new cartridge
    */
    Cartridge();
 
    /**
      Destructor
    */
    virtual ~Cartridge();

    /**
      Query some information about this cartridge.
    */
    static const string& about() { return myAboutString; }

    /**
      Save the internal (patched) ROM image.

      @param out  The output file stream to save the image
    */
    bool save(ofstream& out);

    /**
      Lock/unlock bankswitching capability.
    */
    void lockBank()   { myBankLocked = true;  }
    void unlockBank() { myBankLocked = false; }

  public:
    /**
      The following list contains addressable areas of ROM that are mapped
      to RAM (usually Superchip, but there are other types).  Since such
      RAM is normally mapped in at different addresses for read and write
      ports, read and write offsets must be considered.

      @return  List of addressable RAM areas (can be empty)
    */
    struct RamArea {
      uInt16 start;  uInt16 size;  uInt16 roffset;  uInt16 woffset;
    };
    typedef Common::Array<RamArea> RamAreaList;
    const RamAreaList& ramAreas() { return myRamAreaList; }

  public:
    //////////////////////////////////////////////////////////////////////
    // The following methods are cart-specific and must be implemented
    // in derived classes.
    //////////////////////////////////////////////////////////////////////
    /**
      Set the specified bank.
    */
    virtual void bank(uInt16 bank) = 0;

    /**
      Get the current bank.

      @return  The current bank, or -1 if bankswitching not supported
    */
    virtual int bank() = 0;

    /**
      Query the number of banks supported by the cartridge.
    */
    virtual int bankCount() = 0;

    /**
      Patch the cartridge ROM.

      @param address  The ROM address to patch
      @param value    The value to place into the address
      @return    Success or failure of the patch operation
    */
    virtual bool patch(uInt16 address, uInt8 value) = 0;

    /**
      Access the internal ROM image for this cartridge.

      @param size  Set to the size of the internal ROM image data
      @return  A pointer to the internal ROM image data
    */
    virtual uInt8* getImage(int& size) = 0;

    /**
      Save the current state of this device to the given Serializer.

      @param out  The Serializer object to use
      @return  False on any errors, else true
    */
    virtual bool save(Serializer& out) const = 0;

    /**
      Load the current state of this device from the given Serializer.

      @param in  The Serializer object to use
      @return  False on any errors, else true
    */
    virtual bool load(Serializer& in) = 0;

    /**
      Get a descriptor for the device name (used in error checking).

      @return The name of the object
    */
    virtual string name() const = 0;

  protected:
    /**
      Add the given area to the RamArea list for this cart.

      @param start    The beginning of the RAM area (0x0000 - 0x2000)
      @param size     Total number of bytes of area
      @param roffset  Offset to use when reading from RAM (read port)
      @param woffset  Offset to use when writing to RAM (write port)
    */
    void registerRamArea(uInt16 start, uInt16 size, uInt16 roffset, uInt16 woffset);

  private:
    /**
      Get an image pointer and size for a ROM that is part of a larger,
      multi-ROM image.

      @param image    A pointer to the ROM image
      @param size     The size of the ROM image 
      @param numroms  The number of ROMs in the multicart
      @param md5      The md5sum for the specific cart in the ROM image
      @param id       The ID for the specific cart in the ROM image
      @param settings The settings associated with the system
      @return   The bankswitch type for the specific cart in the ROM image
    */
    static string createFromMultiCart(const uInt8*& image, uInt32& size,
        uInt32 numroms, string& md5, string& id, Settings& settings);

    /**
      Try to auto-detect the bankswitching type of the cartridge

      @param image  A pointer to the ROM image
      @param size   The size of the ROM image 
      @return The "best guess" for the cartridge type
    */
    static string autodetectType(const uInt8* image, uInt32 size);

    /**
      Search the image for the specified byte signature

      @param image      A pointer to the ROM image
      @param imagesize  The size of the ROM image 
      @param signature  The byte sequence to search for
      @param sigsize    The number of bytes in the signature
      @param minhits    The minimum number of times a signature is to be found

      @return  True if the signature was found at least 'minhits' time, else false
    */
    static bool searchForBytes(const uInt8* image, uInt32 imagesize,
                               const uInt8* signature, uInt32 sigsize,
                               uInt32 minhits);

    /**
      Returns true if the image is probably a SuperChip (256 bytes RAM)
    */
    static bool isProbablySC(const uInt8* image, uInt32 size);

    /**
      Returns true if the image is probably a 3F bankswitching cartridge
    */
    static bool isProbably3F(const uInt8* image, uInt32 size);

    /**
      Returns true if the image is probably a 3E bankswitching cartridge
    */
    static bool isProbably3E(const uInt8* image, uInt32 size);

    /**
      Returns true if the image is probably a E0 bankswitching cartridge
    */
    static bool isProbablyE0(const uInt8* image, uInt32 size);

    /**
      Returns true if the image is probably a E7 bankswitching cartridge
    */
    static bool isProbablyE7(const uInt8* image, uInt32 size);

    /**
      Returns true if the image is probably a EF bankswitching cartridge
    */
    static bool isProbablyEF(const uInt8* image, uInt32 size);

    /**
      Returns true if the image is probably a UA bankswitching cartridge
    */
    static bool isProbablyUA(const uInt8* image, uInt32 size);

    /**
      Returns true if the image is probably a 4A50 bankswitching cartridge
    */
    static bool isProbably4A50(const uInt8* image, uInt32 size);

    /**
      Returns true if the image is probably a SB bankswitching cartridge
    */
    static bool isProbablySB(const uInt8* image, uInt32 size);

    /**
      Returns true if the image is probably a 0840 bankswitching cartridge
    */
    static bool isProbably0840(const uInt8* image, uInt32 size);

    /**
      Returns true if the image is probably a CV bankswitching cartridge
    */
    static bool isProbablyCV(const uInt8* image, uInt32 size);

    /**
      Returns true if the image is probably an FE bankswitching cartridge
    */
    static bool isProbablyFE(const uInt8* image, uInt32 size);

    /**
      Returns true if the image is probably an X07 bankswitching cartridge
    */
    static bool isProbablyX07(const uInt8* image, uInt32 size);

  protected:
    // If myBankLocked is true, ignore attempts at bankswitching. This is used
    // by the debugger, when disassembling/dumping ROM.
    bool myBankLocked;

  private:
    // Contains RamArea entries for those carts with accessible RAM.
    RamAreaList myRamAreaList;

    // Contains info about this cartridge in string format
    static string myAboutString;

    // Copy constructor isn't supported by cartridges so make it private
    Cartridge(const Cartridge&);

    // Assignment operator isn't supported by cartridges so make it private
    Cartridge& operator = (const Cartridge&);
};

#endif
