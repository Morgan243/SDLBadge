
## Summary
Useful for debugging game logic and primitive drawing code - but more work needed:

### What's Working
- Only 4-bit and 1-bit image blitting implemented
- Fonts working with 1-bit font
- Basic inputs (note that responsiveness/update rate isn't well simulated)
- Primitive drawing

### What's not Working/Implemented
- Not all image formats supported (2-bit and 8-bit not ported)
- Audio, IR, LEDs

## Building and Using
**Requirements:** Badge Firmware, CMake, SDL2, GNU Tools/Make stack

Tested on linux, but should be portable(ish).

- [Download/clone the Badge Source](https://github.com/HackRVA/Harmony-Badge-2018)
- Sym-link the badge [src/](https://github.com/HackRVA/Harmony-Badge-2018/tree/master/firmware/src) to ./badge_firmware:
    - Change to the directory containing this project (and this README.md file)
    - Run : ```ln -s /path/to/Harmony-Badge-2018/firmware/src ./badge_firmware```
- Generate a make file:
    - Run: ```cmake CMakeLists.txt```
- Build
    - Run: ```make```

There should now be an executable 'BasicSDLBadge' in the directory. 

- D-Pad buttons are mapped to WASD
- Spacebar is mapped to solo botton
- scroll wheel increments the slide percentage
- No mapping yet for the 3 slider buttons
