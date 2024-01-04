# Word Clock Usermod V2

This usermod drives an 16x16 pixel matrix wordclock with WLED. The text
resolution is in 1 minutes and the last line containes seconds. There is some
extra space for more words so i added part of the day and date: the day and
month. The index of the LEDs in the masks always starts at 0, even if the
ledOffset is not 0. There are 3 parameters that control behavior:
 
active: enable/disable usermod
diplayItIs: enable/disable display of "Het is" on the clock
ledOffset: number of LEDs before the wordclock LEDs

## Installation

* `USERMOD_WORDCLOCK_1616NL`   - define this to have this usermod included wled00\usermods_list.cpp

### PlatformIO requirements

No special requirements.

## Full Dutch Matrix

```
  | 0 1 2 3 4 5 6 7 8 9 A B C D E F
--+--------------------------------
0 | H E T   I S   Z E V E N E G E N
1 | K W A R T W E E N V I E R Z E S
2 | V I J F D R I E L F M I N U U T
3 | A C H T W A A L F D E R T I E N
4 | V E E R T I E N   M I N U T E N
5 | V O O R O V E R   H A L F  
6 | T I E N E G E N V I E R V I J F
7 | D R I E L F T W E E N Z E V E N
8 | A C H T W A A L F Z E S   U U R
9 | S . N A C H T S . M I D D A G S
A | S . A V O N D S . M O R G E N S
B | . 0 1 2 3 0 1 2 3 4 5 6 7 8 9 .
C | . . J A N F E B M R T A P R . .
D | . . M E I J U N J U L A U G . .
E | . . S E P O K T N O V D E C . .
F | 0 1 2 3 4 5 0 1 2 3 4 5 6 7 8 9
```

## Change Log

2023/12/25 cloned from usermod_v2_word_clock
2023/12/26 fixed day-part postition
2024/01/04 fixed wrong pixel index for "tien"

