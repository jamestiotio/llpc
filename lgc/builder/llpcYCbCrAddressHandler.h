/*
 ***********************************************************************************************************************
 *
 *  Copyright (c) 2020 Advanced Micro Devices, Inc. All Rights Reserved.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *
 **********************************************************************************************************************/
 /**
  ***********************************************************************************************************************
  * @file  llpcYCbCrAddressHandler.h
  * @brief LLPC header file: contains the definition of LLPC class YCbCrAddressHandler.
  ***********************************************************************************************************************
  */

#pragma once
#include "llpcGfxRegHandler.h"
#include "llpcBuilderImpl.h"

namespace lgc
{

// =====================================================================================================================
// The class is used for calculating and mantain the base address of each plane in YCbCr image.
// Note: There are at most 3 planes, and the index for plane is start from zero
class YCbCrAddressHandler
{
public:
    YCbCrAddressHandler(Builder* builder, SqImgRsrcRegHandler* sqImgRsrcRegHelper, GfxIpVersion* gfxIp)
    {
        pBuilder = builder;
        pRegHelper = sqImgRsrcRegHelper;
        pGfxIp = gfxIp;
        planeBaseAddresses.clear();
        pOne = builder->getInt32(1);
    }

    // Generate base address for image planes
    void genBaseAddress(unsigned planeCount);

    // Generate height and pitch
    void genHeightAndPitch(unsigned bits, unsigned bpp, unsigned xBitCount, bool isTileOptimal, unsigned planeNum);

    // Power2Align operation
    llvm::Value* power2Align(llvm::Value* x, unsigned align);

    // Get specific plane
    llvm::Value* getPlane(unsigned idx) { assert(idx < 3); return planeBaseAddresses[idx]; }

    // Get pitch for Y plane
    llvm::Value* getPitchY() { return pPitchY; }

    // Get pitch for Cb plane
    llvm::Value* getPitchCb() { return pPitchCb; }

    // Get height for Y plane
    llvm::Value* getHeightY() { return pHeightY; }

    // Get Height for Cb plane
    llvm::Value* getHeightCb() { return pHeightCb; }

    SqImgRsrcRegHandler*               pRegHelper;
    Builder*                           pBuilder;
    llvm::SmallVector<llvm::Value*, 3> planeBaseAddresses;
    llvm::Value*                       pPitchY;
    llvm::Value*                       pHeightY;
    llvm::Value*                       pPitchCb;
    llvm::Value*                       pHeightCb;
    llvm::Value*                       pOne;
    GfxIpVersion*                      pGfxIp;
};

} // lgc