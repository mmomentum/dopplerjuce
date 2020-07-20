/*
  ==============================================================================

	LookAndFeel.h
	Created: 18 Jul 2020 12:20:19pm
	Author:  Aidan

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

#include "InterfaceDefines.h"


namespace look {
	class sliderXYLAF : public juce::LookAndFeel_V4 {
	public:
		void drawLinearSlider(Graphics& g, int x, int y, int width, int height,
			float sliderPos,
			float minSliderPos,
			float maxSliderPos,
			const Slider::SliderStyle style, Slider& slider)
		{
			auto isTwoVal = (style == Slider::SliderStyle::TwoValueVertical || style == Slider::SliderStyle::TwoValueHorizontal);
			auto isThreeVal = (style == Slider::SliderStyle::ThreeValueVertical || style == Slider::SliderStyle::ThreeValueHorizontal);

			auto trackWidth = jmin(6.0f, slider.isHorizontal() ? (float)height * 0.25f : (float)width * 0.25f);

			Point<float> startPoint(slider.isHorizontal() ? (float)x : (float)x + (float)width * 0.5f,
				slider.isHorizontal() ? (float)y + (float)height * 0.5f : (float)(height + y));

			Point<float> endPoint(slider.isHorizontal() ? (float)(width + x) : startPoint.x,
				slider.isHorizontal() ? startPoint.y : (float)y);

			Path backgroundTrack;
			backgroundTrack.startNewSubPath(startPoint);
			backgroundTrack.lineTo(endPoint);
			g.setColour(whiteColor.withAlpha(0.075f));
			g.strokePath(backgroundTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });

			Path valueTrack;
			Point<float> minPoint, maxPoint, midPoint, thumbPoint;


			auto kx = slider.isHorizontal() ? sliderPos : ((float)x + (float)width * 0.5f);
			auto ky = slider.isHorizontal() ? ((float)y + (float)height * 0.5f) : sliderPos;

			midPoint.setXY(slider.getWidth() / 2.0f, slider.getHeight() / 2.0f);

			minPoint = startPoint;
			maxPoint = { kx, ky };

			auto thumbWidth = getSliderThumbRadius(slider);

			valueTrack.startNewSubPath(midPoint);
			valueTrack.lineTo(isThreeVal ? thumbPoint : maxPoint);

			// draw track

			g.setColour(whiteColor);
			g.strokePath(valueTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });

			// draw dot at midpoint

			g.fillEllipse(Rectangle<float>(static_cast<float> (trackWidth), static_cast<float> (trackWidth)).withCentre(isThreeVal ? midPoint : maxPoint));
		}

	};
}