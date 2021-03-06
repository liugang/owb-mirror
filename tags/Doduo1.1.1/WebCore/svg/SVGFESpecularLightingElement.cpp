/*
    Copyright (C) 2004, 2005, 2007 Nikolas Zimmermann <zimmermann@kde.org>
                  2004, 2005, 2006 Rob Buis <buis@kde.org>
                  2005 Oliver Hunt <oliver@nerget.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "config.h"

#if ENABLE(SVG) && ENABLE(SVG_FILTERS)
#include "SVGFESpecularLightingElement.h"

#include "RenderObject.h"
#include "SVGColor.h"
#include "SVGNames.h"
#include "SVGFELightElement.h"
#include "SVGParserUtilities.h"
#include "SVGResourceFilter.h"

namespace WebCore {

SVGFESpecularLightingElement::SVGFESpecularLightingElement(const QualifiedName& tagName, Document* doc)
    : SVGFilterPrimitiveStandardAttributes(tagName, doc)
    , m_in1(this, SVGNames::inAttr)
    , m_specularConstant(this, SVGNames::specularConstantAttr, 1.0f)
    , m_specularExponent(this, SVGNames::specularExponentAttr, 1.0f)
    , m_surfaceScale(this, SVGNames::surfaceScaleAttr, 1.0f)
    , m_kernelUnitLengthX(this, SVGNames::kernelUnitLengthAttr)
    , m_kernelUnitLengthY(this, SVGNames::kernelUnitLengthAttr)
    , m_filterEffect(0)
{
}

SVGFESpecularLightingElement::~SVGFESpecularLightingElement()
{
}

void SVGFESpecularLightingElement::parseMappedAttribute(MappedAttribute* attr)
{    
    const String& value = attr->value();
    if (attr->name() == SVGNames::inAttr)
        setIn1BaseValue(value);
    else if (attr->name() == SVGNames::surfaceScaleAttr)
        setSurfaceScaleBaseValue(value.toFloat());
    else if (attr->name() == SVGNames::specularConstantAttr)
        setSpecularConstantBaseValue(value.toFloat());
    else if (attr->name() == SVGNames::specularExponentAttr)
        setSpecularExponentBaseValue(value.toFloat());
    else if (attr->name() == SVGNames::kernelUnitLengthAttr) {
        float x, y;
        if (parseNumberOptionalNumber(value, x, y)) {
            setKernelUnitLengthXBaseValue(x);
            setKernelUnitLengthYBaseValue(y);
        }
    } else
        SVGFilterPrimitiveStandardAttributes::parseMappedAttribute(attr);
}

SVGFilterEffect* SVGFESpecularLightingElement::filterEffect(SVGResourceFilter* filter) const
{
    ASSERT_NOT_REACHED();
    return 0;
}

LightSource* SVGFESpecularLightingElement::findLights() const
{
    LightSource* light = 0;
    for (Node* n = firstChild(); n; n = n->nextSibling()) {
        if (n->hasTagName(SVGNames::feDistantLightTag) ||
            n->hasTagName(SVGNames::fePointLightTag) ||
            n->hasTagName(SVGNames::feSpotLightTag)) {
            SVGFELightElement* lightNode = static_cast<SVGFELightElement*>(n); 
            light = lightNode->lightSource();
            break;
        }
    }

    return light;
}

bool SVGFESpecularLightingElement::build(FilterBuilder* builder)
{
    FilterEffect* input1 = builder->getEffectById(in1());
    
    if(!input1)
        return false;
        
    RenderStyle* parentStyle = this->styleForRenderer(parent()->renderer());    
    RenderStyle* filterStyle = this->resolveStyle(parentStyle);
    
    Color color = filterStyle->svgStyle()->lightingColor();
    
    parentStyle->deref(document()->renderArena());
    filterStyle->deref(document()->renderArena());
    
    RefPtr<FilterEffect> addedEffect = FESpecularLighting::create(input1, color, surfaceScale(), specularConstant(), 
                                        specularExponent(), kernelUnitLengthX(), kernelUnitLengthY(), findLights());
    builder->add(result(), addedEffect.release());

    return true;
}

}

#endif // ENABLE(SVG)
