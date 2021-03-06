/*
    Copyright (C) 2004, 2005, 2006 Nikolas Zimmermann <wildfox@kde.org>
                  2004, 2005 Rob Buis <buis@kde.org>
                  2005 Eric Seidel <eric.seidel@kdemail.net>

    This file is part of the KDE project

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#ifndef SVGFEColorMatrix_h
#define SVGFEColorMatrix_h

#ifdef SVG_SUPPORT
#include "SVGFilterEffect.h"
#include "SVGRenderTreeAsText.h"

namespace WebCore {

enum SVGColorMatrixType {
    SVG_FECOLORMATRIX_TYPE_UNKNOWN          = 0,
    SVG_FECOLORMATRIX_TYPE_MATRIX           = 1,
    SVG_FECOLORMATRIX_TYPE_SATURATE         = 2,
    SVG_FECOLORMATRIX_TYPE_HUEROTATE        = 3,
    SVG_FECOLORMATRIX_TYPE_LUMINANCETOALPHA = 4
};

class SVGFEColorMatrix : public SVGFilterEffect {
public:
    SVGColorMatrixType type() const;
    void setType(SVGColorMatrixType);

    const Vector<float>& values() const;
    void setValues(const Vector<float>&);

    virtual TextStream& externalRepresentation(TextStream&) const;

#if PLATFORM(CI)
    virtual CIFilter* getCIFilter(SVGResourceFilter*) const;
#endif

private:
    SVGColorMatrixType m_type;
    Vector<float> m_values;
};

} // namespace WebCore

#endif // SVG_SUPPORT

#endif // SVGFEColorMatrix_h
