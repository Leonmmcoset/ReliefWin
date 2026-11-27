/** @file
  EFI_RNG_PROTOCOL from UEFI 2.4 and EDK II MdePkg/Include/{Protocol,Guid}/Rng.h.

  Copyright (c) 2013 - 2018, Intel Corporation. All rights reserved.<BR>
  Copyright (c) Microsoft Corporation.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#pragma once

#define EFI_RNG_PROTOCOL_GUID \
    { 0x3152bca5, 0xeade, 0x433d, { 0x86, 0x2e, 0xc0, 0x1c, 0xdc, 0x29, 0x1f, 0x44 } }

#define EFI_RNG_ALGORITHM_RAW \
    { 0xe43176d7, 0xb6e8, 0x4827, { 0xb7, 0x84, 0x7f, 0xfd, 0xc4, 0xb6, 0x85, 0x61 } }

typedef EFI_GUID EFI_RNG_ALGORITHM;
typedef struct _EFI_RNG_INTERFACE EFI_RNG_INTERFACE;

typedef EFI_STATUS (EFIAPI *EFI_RNG_GET_INFO)(
    IN EFI_RNG_INTERFACE *This,
    IN OUT UINTN *RNGAlgorithmListSize,
    OUT EFI_RNG_ALGORITHM *RNGAlgorithmList);

typedef EFI_STATUS (EFIAPI *EFI_RNG_GET_RNG)(
    IN EFI_RNG_INTERFACE *This,
    IN EFI_RNG_ALGORITHM *RNGAlgorithm OPTIONAL,
    IN UINTN RNGValueLength,
    OUT UINT8 *RNGValue);

struct _EFI_RNG_INTERFACE
{
    EFI_RNG_GET_INFO GetInfo;
    EFI_RNG_GET_RNG GetRNG;
};

typedef EFI_RNG_INTERFACE EFI_RNG_PROTOCOL;
