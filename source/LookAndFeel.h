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

			g.setGradientFill(ColourGradient(whiteColor, maxPoint.getX(), maxPoint.getY(),
				Colours::transparentWhite, midPoint.getX(), midPoint.getY(), false));

			g.strokePath(valueTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });

			// draw dot at midpoint

			g.setColour(whiteColor);

			g.fillEllipse(juce::Rectangle<float>(static_cast<float> (trackWidth), static_cast<float> (trackWidth)).withCentre(isThreeVal ? midPoint : maxPoint));
		}

	};

	class ButtonLAF : public juce::LookAndFeel_V4 {

		void drawButtonBackground(Graphics& g,
			Button& button,
			const Colour& backgroundColour,
			bool shouldDrawButtonAsHighlighted,
			bool shouldDrawButtonAsDown)
		{
			auto cornerSize = 3.0f;
			auto bounds = button.getLocalBounds().toFloat().reduced(0.5f, 0.5f);

			auto baseColour = Colours::transparentWhite;

			if (shouldDrawButtonAsHighlighted)
				baseColour = leftColor.withAlpha(0.5f);

			if (button.getToggleState() == true)
				baseColour = greyColor.withAlpha(0.5f);

			if (shouldDrawButtonAsHighlighted && button.getToggleState() == true)
				baseColour = leftColor.interpolatedWith(greyColor, 0.5f).withAlpha(0.5f);

			g.setColour(baseColour);

			auto flatOnLeft = button.isConnectedOnLeft();
			auto flatOnRight = button.isConnectedOnRight();
			auto flatOnTop = button.isConnectedOnTop();
			auto flatOnBottom = button.isConnectedOnBottom();

			if (flatOnLeft || flatOnRight || flatOnTop || flatOnBottom)
			{
				Path path;
				path.addRoundedRectangle(bounds.getX(), bounds.getY(),
					bounds.getWidth(), bounds.getHeight(),
					cornerSize, cornerSize,
					!(flatOnLeft || flatOnTop),
					!(flatOnRight || flatOnTop),
					!(flatOnLeft || flatOnBottom),
					!(flatOnRight || flatOnBottom));

				g.fillPath(path);

				g.setColour(greyColor.withAlpha(0.5f));
				g.strokePath(path, PathStrokeType(2.0f));
			}
			else
			{
				g.fillRoundedRectangle(bounds.reduced(1.0f), cornerSize);

				if (button.getToggleState() == false)
					g.setColour(greyColor.withAlpha(0.5f));

				g.drawRoundedRectangle(bounds.reduced(1.0f), cornerSize, 2.0f);
			}
		}

		void drawToggleButton(Graphics& g, ToggleButton& button,
			bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
		{
			auto fontSize = jmin(15.0f, (float)button.getHeight() * 0.75f);
			auto tickWidth = fontSize * 1.1f;

			drawTickBox(g, button, 4.0f, ((float)button.getHeight() - tickWidth) * 0.5f,
				tickWidth, tickWidth,
				button.getToggleState(),
				button.isEnabled(),
				shouldDrawButtonAsHighlighted,
				shouldDrawButtonAsDown);

			g.setColour(button.findColour(ToggleButton::textColourId));
			g.setFont(fontSize);

			if (!button.isEnabled())
				g.setOpacity(0.5f);

			g.drawFittedText(button.getButtonText(),
				button.getLocalBounds().withTrimmedLeft(roundToInt(tickWidth) + 10)
				.withTrimmedRight(2),
				Justification::centredLeft, 10);
		}


		void drawTickBox(Graphics& g, Component& component,
			float x, float y, float w, float h,
			const bool ticked,
			const bool isEnabled,
			const bool shouldDrawButtonAsHighlighted,
			const bool shouldDrawButtonAsDown)
		{
			ignoreUnused(isEnabled, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);

			Rectangle<float> tickBounds(x, y, w, h);

			g.setColour(component.findColour(ToggleButton::tickDisabledColourId));
			g.drawRoundedRectangle(tickBounds, .0f, 1.0f);

			if (ticked)
			{
				g.setColour(component.findColour(ToggleButton::tickColourId));
				auto tick = getTickShape(0.75f);
				g.fillPath(tick, tick.getTransformToScaleToFit(tickBounds.reduced(4, 5).toFloat(), false));
			}
		}

		void changeToggleButtonWidthToFitText(ToggleButton& button)
		{
			auto fontSize = jmin(15.0f, (float)button.getHeight() * 0.75f);
			auto tickWidth = fontSize * 1.1f;

			Font font(fontSize);

			button.setSize(font.getStringWidth(button.getButtonText()) + roundToInt(tickWidth) + 14, button.getHeight());
		}
	};

	class TooltipLAF : public juce::LookAndFeel_V4
	{
		void drawTooltip(Graphics& g, const String& text, int width, int height)
		{
			juce::Rectangle<int> bounds(width, height);
			auto cornerSize = 5.0f;

			g.setColour(screenColor);
			g.fillRoundedRectangle(bounds.toFloat(), 0);

			g.setColour(whiteColor);
			g.setFont(Font("Museo Moderno", 12.00f, Font::plain));
			g.drawFittedText(text, bounds, juce::Justification::centred, 1);
		}
	};

	class SliderLAF : public juce::LookAndFeel_V4
	{
		void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
			const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider)
		{
			auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(5);

			auto radius = jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
			auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
			auto lineW = radius * 0.16;
			auto arcRadius = radius - lineW * 0.5f;

			Path backgroundArc;
			backgroundArc.addCentredArc(bounds.getCentreX(),
				bounds.getCentreY(),
				arcRadius,
				arcRadius,
				0.0f,
				rotaryStartAngle,
				rotaryEndAngle,
				true);

			g.setColour(greyColor.withAlpha(float(0.25)));
			g.strokePath(backgroundArc, PathStrokeType(lineW, PathStrokeType::curved, PathStrokeType::rounded));

			if (slider.isEnabled())
			{
				Path valueArc;
				valueArc.addCentredArc(bounds.getCentreX(),
					bounds.getCentreY(),
					arcRadius,
					arcRadius,
					0.0f,
					rotaryStartAngle,
					toAngle,
					true);

				g.setColour(whiteColor);
				g.strokePath(valueArc, PathStrokeType(lineW, PathStrokeType::curved, PathStrokeType::rounded));
			}

			auto thumbWidth = lineW * 1.75f;

			Point<float> thumbPoint(bounds.getCentreX() + arcRadius * std::cos(toAngle - MathConstants<float>::halfPi),
				bounds.getCentreY() + arcRadius * std::sin(toAngle - MathConstants<float>::halfPi));

			g.setColour(backgroundColor);
			g.fillEllipse(juce::Rectangle<float>(thumbWidth, thumbWidth).withCentre(thumbPoint));

			g.setColour(whiteColor);
			g.fillEllipse(juce::Rectangle<float>(lineW, lineW).withCentre(thumbPoint));
		}
	};
}