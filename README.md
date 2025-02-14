# Master Fears Not Bullets

Author: Tianjian Huang <thuang2@andrew.cmu.edu>
  
Design: 
This game's idea is from Sekiro. It (partly) implements the core mechanism of the game: deflecting attacks.
  
Screen Shot:
  
![Screen Shot](screenshot.png)
  
How Your Asset Pipeline Works:

1. Source asset files are png files generated by GIMP; 
2. Load png files via load_png();
3. According to each pixel's brightness, construct tile table;
4. Construct different asset's palette table;
5. Load assets to ppu at runtime.
  
All source are loaded only once when the game mode is initialized.
  
  
How To Play:
  
Player is on the left side, and the enemy is on the right side.  
The enemy will shoot you, so click RMB to defense.  
If you guard at a perfect timing (right before the bullet gonna hit you), the mysterious master power will keep bullets from you. Otherwise, prepare to get hit!  
However, if you try to click defense button in a high frequency, or hold the button like a coward, 
you will lose your power temporarily and become vulnerable.  
(Just like in Sekiro: if you multi-click defense button in a very short period, your i-frames will be shortened.
If you hold the defense button, it is not a deflection and you may still bear damages.)  
When you successfully deflect a bullet, you gain 1 score. When you get hit, you lose 1 health.  
The goal is to deflect bullets as many as possible within 60 seconds, and do not lose all 3 healths.  
  
Sources:   
https://github.com/ericeschnei/15-466-f20-base1, asset tiles 0-14, and how to organize them in PPU466.  
https://www.sekirothegame.com/, the Sekiro official website.
  
This game was built with [NEST](NEST.md).  

