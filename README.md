# FlipperZeroIRPlayer
Program for playing Flipper Zero IR files on CardPuter device in Arduino Sketch

![Screenshot](https://github.com/James-P-D/FlipperZeroIRPlayer/blob/main/screenshot.gif)

# Usage

Simply copy the relevent infrared file from your Flipper Zero SD card to a new FAT32 formatted SD card and insert into the CardPuter. After running the app use the keyboard to enter the path to the file and press enter.

For example, given a Flipper IR file called `off.ir` containing the following:

```
Filetype: IR signals file
Version: 1
# 
# Universal Power Off (original filename UNIVERSAL_POWER_OFF_DEVICES.IR)
#
name: SAMSUNG
type: parsed
protocol: Samsung32
address: 07 00 00 00
command: 02 00 00 00
# 
```

Save the file in a folder named `ir` and then on the CardPuter enter `/ir/off.ir` to transmit the signal.

Note that some Flipper IR files contain `raw` data rather than parsed:

```
name: PANASONIC
type: raw
frequency: 38000
duty_cycle: 0.330000
data: 3481 1715 457 442 428 1284 457 442 428 443 427 443 427 443 427 442 428 442 428 442 453 417 453 417 453 417 452 418 451 1289 451 422 448 447 422 424 447 447 423 448 422 424 446 448 422 448 422 448 422 1319 422 448 422 448 422 448 422 448 422 448 423 448 422 448 422 448 422 1319 422 448 422 1319 422 1319 422 1319 422 1319 422 448 422 449 421 1319 422 448 422 1320 421 1319 422 1320 421 1319 422 449 422 1319 422 74732 3475 1750 422 448 422 1319 422 448 422 448 422 448 422 448 422 448 422 448 422 448 422 448 422 448 422 448 422 449 422 1319 422 449 421 449 421 448 422 449 421 449 421 449 421 449 421 449 421 449 421 1320 421 449 421 449 421 449 421 449 421 449 422 449 421 449 422 449 421 1320 421 449 421 1320 421 1320 421 1320 421 1320 421 449 421 449 421 1320 421 449 421 1320 421 1320 421 1320 421 1320 421 450 420 1321 420 74732 3475 1750 422 448 422 1319 422 448 422 448 423 448 422 448 422 448 422 448 422 448 422 448 422 448 422 448 422 448 422 1319 422 448 422 448 422 448 422 448 422 448 422 449 421 449 421 449 421 448 422 1320 421 449 421 449 421 449 422 449 421 449 421 449 422 449 421 449 421 1320 421 449 421 1320 421 1320 421 1320 421 1320 421 449 421 449 421 1320 421 449 421 1320 421 1320 421 1320 421 1320 421 449 421 1320 421
```

Sadly, I've so far been unable to transmit this on the CardPuter. Will try and get this resolved at some point.