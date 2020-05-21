# Scorched 3D

A simple turn-based artillery game and also a real-time strategy game.

## Overview

Scorched3D incorporates lively, fully destructible 3D landscapes that include animated jets, naval vessels, water and even birds, with detailed tanks and projectiles and stunning visual effects.

A simple turn-based artillery game and also a real-time strategy game in which players can counter each others' weapons with other creative accessories, shields and tactics. Test your skill in timing, aiming and judgement of distance, move around on the landscape, or outsmart your opponent economically.

### Features

  * Realisticly rendered 3D landscape environments
  * Realtime or turn based artillery combat
  * Online or offline, single or multi-player play
  * Mod support for various gaming styles
  * Tournament and stats championships

## About

This repo was forked over to GitHub on Thursday, March 22nd, 2018 for experimentation and further enhancement.

## Development

Download and install the c++ development tools for your Linux distribution.

### Dependencies

Download the development dependencies.

#### Arch Linux

  * openal
  * wxGTK
  * wxgtk-common
  * freealut
  * fftw3
  * sdl_net
  * wxgtk2
  * wxgtk3
  * freetype2

### Configure

``` bash
cd scorched
automake --add-missing
sh autogen.sh
```

### Compile

``` bash
make -j2
```

### Install

``` bash
sudo make install
```
