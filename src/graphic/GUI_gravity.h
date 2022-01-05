//
// Created by AmirHossein Aghajari on 12/31/21.
//

#ifndef GRAVITY_CENTER_H
#define GRAVITY_CENTER_H (0b001)
#define GRAVITY_LEFT (0b011)
#define GRAVITY_RIGHT (0b101)
#define GRAVITY_MASK_H (0b111)

#define GRAVITY_CENTER_V (0b001 << 3)
#define GRAVITY_TOP (0b011 << 3)
#define GRAVITY_BOTTOM (0b101 << 3)
#define GRAVITY_MASK_V (0b111 << 3)

#define GRAVITY_CENTER (GRAVITY_CENTER_V | GRAVITY_CENTER_H)
#define GRAVITY_FILL (0b111111)
#define GRAVITY_FILL2 (0b1111111)

#endif