# DatafileLibrary

This library is a recreation of the DATAFILE object from Allegro pre-version 5.

It uses a json-like manifest file to specify what should be in the datafile when loaded.

## Dependencies:
- Allegro 5 Library
  - Font Addon
  - TTF Addon
  - PhysFS Addon
  - Image Addon

## Valid Object Types
 - Bitmap
 - Font (Bitmap or TTF)
 - Text (std::string)
 - Datfile (nested)

 ## DSON File Format
 (see example.ini)
