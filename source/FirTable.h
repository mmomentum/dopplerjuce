/*
  ==============================================================================

    FirTable.h
    Created: 25 Jul 2020 3:46:44pm
    Author:  Aidan

    instead of running a complicated equation to get a product,
    we choose to precalculate all of the values we would use
    and store them here for better speed
    
  ==============================================================================
*/

#pragma once

class FirTable
{
 public:
  static float fir2[128];
  static float fir4[128];
  static float fir6[128];
  static float fir8[128];

  static float fir18[128];
  static float fir20[128];
  static float fir22[128];
  static float fir24[128];

  static float fir34[128];
  static float fir36[128];
  static float fir38[128];
  static float fir40[128];
  
  static float fir48[128];
};
