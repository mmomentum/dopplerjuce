/*
  ==============================================================================

    InterfaceDefines.h
    Created: 7 May 2020 12:14:36am
    Author:  Aidan

    Constants that define vector graphics within the interface. All done here for 
    the sake of centralization. We can also add other interface related defs and
    constants in here, like colors, or fonts.
    
  ==============================================================================
*/

#include "Juceheader.h"

#pragma once

// nescessary constants

#define BORDER_SIZE					20

// main panel size

#define MAIN_PANEL_WIDTH			400
#define MAIN_PANEL_HEIGHT			520

// component sizes (before borders)

#define XY_PAD_WIDTH				400
#define XY_PAD_HEIGHT				400

#define PARAMETER_PANEL_WIDTH		400
#define PARAMETER_PANEL_HEIGHT		120

// colors
const Colour screenColor = Colour(0xFF1A1A1A); // almost black
const Colour screenColorGradient = Colour(0xFF272727); // gradient black (for screens)
const Colour backgroundColor = Colour(43, 48, 58); // dark grey
const Colour greyColor = Colour(124, 124, 124); // light grey
const Colour whiteColor = Colour(238, 229, 233); // off-white
const Colour leftColor = Colour(214, 73, 51); // a nice orange
const Colour rightColor = Colour(146, 220, 229); // light blue

// fonts

const Font font1("MuseoModerno", 12.00f, Font::plain);
const Font monospace("Roboto Mono", 12.00f, Font::plain);
