/* Copyright (c) 2007-2012 Eliot Eshelman
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef SIMPLEX_H_
#define SIMPLEX_H_

#include "fixed.h"

// Multi-octave Simplex noise
// For each octave, a higher frequency/lower amplitude function will be added to the original.
// The higher the persistence [0-1], the more of each succeeding octave will be added.
float octave_noise_2d(const float octaves,
                    const float persistence,
                    const float scale,
                    const float x,
                    const float y);

// Raw Simplex noise - a single noise value.
float raw_noise_2d(const float x, const float y);

// Multi-octave Simplex noise
// For each octave, a higher frequency/lower amplitude function will be added to the original.
// The higher the persistence [0-1], the more of each succeeding octave will be added.
fixed octave_noise_2d_fixed(const int octaves,
                    const fixed persistence,
                    const fixed scale,
                    const fixed x,
                    const fixed y);

// Raw Simplex noise - a single noise value.
fixed raw_noise_2d_fixed(const fixed x, const fixed y);


#endif /*SIMPLEX_H_*/
