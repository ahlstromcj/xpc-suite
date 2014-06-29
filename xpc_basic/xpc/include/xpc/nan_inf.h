#ifndef PSX_NAN_INF_H
#define PSX_NAN_INF_H

/******************************************************************************
 * Module:        nan_inf.h -- Chris Ahlstrom 07/03/2005
 *-----------------------------------------------------------------------------
 *
 * Namespace:     C (global)
 * Library:       libpsxc
 * Class:         C::NAN and INF support
 *
 * \license
 *    Float-Aid of POSIX C Wrapper (PSXC) library
 *    Copyright (C) 2005-2007 by Chris Ahlstrom
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or (at
 *    your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful, but
 *    WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 *    02110-1301, USA.
 *
 *------------------------------------------------------------------------*//**
 *
 * \file
 *    This module provides items that make the usage and comparison of
 *    floating-point numbers a little more foolproof.
 *
 * \updates
 *    07/03/2005-08/30/2007
 *
 *    \a nan_inf provides more fool-proof usage of floating-point values.
 *
 *    This module supports the concepts of huge values, not-a-number,
 *    infinity, and some others.
 *
 *    See nan_inf.c for full details.
 *
 *//*-------------------------------------------------------------------------*/

#include <psx/c/macros.h>        /* required for defining GNU macros, etc.    */
PSX_REVISION_DECL(nan_inf)       /* extern void show_nan_inf_info()           */

/******************************************************************************
 * NAN_INF_EPSILON
 *------------------------------------------------------------------------*//**
 *
 *    Provides a default "epsilon" value.  We need to provide a source that
 *    shows this is a good value to use!
 *
 *//*-------------------------------------------------------------------------*/

#define NAN_INF_EPSILON   0.000001

/******************************************************************************
 * Global functions
 *----------------------------------------------------------------------------*/

EXTERN_C_DEC

extern double psx_float_nan (void);
extern double psx_float_infinite (void);
extern cbool_t psx_is_zero (double x);
extern cbool_t psx_is_infinite (double x);
extern cbool_t psx_is_huge (double x);
extern cbool_t psx_is_minus_huge (double x);
extern cbool_t psx_is_nan (double x);
extern cbool_t psx_is_neq (double x, double y);
extern cbool_t psx_is_eq (double x, double y);
extern cbool_t psx_is_lt_or_eq (double x, double y);
extern cbool_t psx_is_gt_or_eq (double x, double y);
extern cbool_t psx_psx_is_neq_epsilon (double x, double y, double epsilon);
extern cbool_t psx_psx_is_eq_epsilon (double x, double y, double epsilon);
extern cbool_t psx_psx_is_lt_or_eq_epsilon (double x, double y, double epsilon);
extern cbool_t psx_psx_is_gt_or_eq_epsilon (double x, double y, double epsilon);

EXTERN_C_END

#endif         /* PSX_NAN_INF_H */

/******************************************************************************
 * End of nan_inf.h
 *----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=c
 *//*-------------------------------------------------------------------------*/
