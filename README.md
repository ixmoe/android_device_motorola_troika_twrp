# TWRP Device configuration for Motorola Moto One Vision

## Device specifications

Basic   | Spec Sheet
-------:|:-------------------------
CPU     | Octa-core 2.2 GHz
CHIPSET | Samsung Exynos 9609
GPU     | Mali G72 MP3
Memory  | 4 / 6GB
Shipped Android Version | 9.0 (Pie)
Storage | 128GB
Battery | 3500 mAh
Dimensions | 160.1 x 71.2 x 8.7 mm
Display | 1080 x 2520 pixels, 6.3" LTPS IPS LCD
Rear Camera  | 48 MP (f/1.7) + 5 MP (f/2.2), (PDAF, dual pixel)
Front Camera | 25 MP (f/2.0)

![Device Picture](https://cdn2.gsmarena.com/vv/pics/motorola/motorola-one-vision-0.jpg)

### Kernel Source
Check here: https://github.com/ixmoe/android_kernel_motorola_universal9610

### How to compile

```sh
. build/envsetup.sh
lunch omni_kane-eng
make -j4 recoveryimage
```
### Copyright
 ```
  /*
  *  Copyright (C) 2013-17 The OmniROM Project
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
  *
  */
  ```
