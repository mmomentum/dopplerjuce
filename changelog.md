### 26/05/2020
#### aidan
- realised today while making the velocity calculator that the interpolation timer system stops working when the GUI is closed because plugineditor files are only supposed to be there for the visuals, they stop running when the GUI is closed.
- i'll need to fix this by moving the timer / interpolator to the pluginprocessor.cpp file, and somehow "linking" the dot locations to eachother via a global variable so that visual feedback still works.
---
### 24/05/2020
#### aidan
- spent the last few days finding VSTs and code to reverse engineer for the pitch shifting / delay systems and it seems like i've found both; 
- for the pitch shifting system i will need to determine constants for the FFT settings as well as set up a system for determining pitch shift based on velocity.
- for the delay, VCV rack's fundamental delay acts exactly as we need it to, and it's code is open source.
--- 
### 21/05/2020
#### aidan
- implemented basic functionality for filtering and attenuation within the processing system
- the current math for calculating filtering / attenuation is in no way accurate to what we would need for a real spatializer
---
### 19/05/2020
#### aidan
- made a new repository with a different root file (so now the .jucer is included and cross-platform / cross-developer use is more streamlined and there will be less of a hassle)
---
### 18/05/2020
#### aidan
- added a delay calculation function within pluginprocessor.cpp
---
### 17/05/2020
#### aidan
- got global variables working (using the **extern** definition)
- added a function for distance calculation within pluginprocessor.cpp
---
### 16/05/2020
#### aidan
- adjust the interpolator to be exponential (time parameter is now inaccurate now but fixing it might be easily doable)
- working on global variables for working inbetween xypad.cpp and pluginprocessor.cpp
---
### 15/05/2020
#### aidan
- move **~destructor** functions to the end of all .cpp files for better comprehension
- implemented a linear interpolation system for the *actual* location of the sound emitter (need to implement parameter smoothing for the DSP portion of the project)
---
### 10/05/2020
#### aidan
- improved slider handling: both methods of setting emitter position work now
- refactored a bit of the xypad code
---
### 09/05/2020
#### aidan
- fixed the parameter system through finding an embarrasing error (i forgot to change names for the function associated with creating additional parameter pointers)
- doing **setstate()** and **savestate()** stuff for the plugin seems like it needs to calculate parameters to actually set so doing that will come after most of the DSP is done
---
### 08/05/2020
#### aidan
- got the additional parameter panel working. the *actual internals* of the parameter system are still kind of broken so that is tomorrow's job
---
### 07/05/2020
#### aidan
- fixed the parameter system along with the weird getWidth() exception throwing bug created by something involving linking components with the help of a friend who actually understands C++.
---
### 06/05/2020
#### aidan
- began seperating graphical elements into **components** with thier own seperate files. now need to fix parameters / listeners.
---
### 04/05/2020
#### aidan 
- fixed the delay system's buffer size allocation from **sample rate x block size** to just **sample rate**. allocation prior to this took up around 180 megabytes and was seven minutes long. lol
---
### 02/05/2020
#### aidan
- implemented a simple delay system. it currently works with a circular buffer so that input buffer storage into the delay buffer never ends
- started work on the actual dot vs target dot GUI element and havent worked out how to deal with the parameters for doing that
---
### 01/05/2020
#### aidan
- changed x / y axis sliders range from **0 - 1** to **-1 - 1** for a more accurate cartesian coordinate experience on both the sliders themselves as well as the parameters
