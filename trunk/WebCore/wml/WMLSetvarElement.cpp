/**
 * Copyright (C) 2008 Torch Mobile Inc. All rights reserved.
 *               http://www.torchmobile.com/
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include "config.h"

#if ENABLE(WML)
#include "WMLSetvarElement.h"

#include "HTMLNames.h"
#include "WMLErrorHandling.h"
#include "WMLTaskElement.h"
#include "WMLVariables.h"

namespace WebCore {

WMLSetvarElement::WMLSetvarElement(const QualifiedName& tagName, Document* doc)
    : WMLElement(tagName, doc)
{
}

WMLSetvarElement::~WMLSetvarElement()
{
}

void WMLSetvarElement::parseMappedAttribute(MappedAttribute* attr)
{
    if (attr->name() == HTMLNames::nameAttr) {
        const AtomicString& value = attr->value();
        String name;

        bool isValid = isValidVariableName(value, true);
        if (isValid) {
            name = substituteVariableReferences(value, document());
            isValid = isValidVariableName(name, true);
        }

        if (isValid)
            m_name = name;
        else
            reportWMLError(document(), WMLErrorInvalidVariableName);
    } else if (attr->name() == HTMLNames::valueAttr)
        m_value = substituteVariableReferences(attr->value(), document());
    else
        WMLElement::parseMappedAttribute(attr);

}

void WMLSetvarElement::insertedIntoDocument()
{
    WMLElement::insertedIntoDocument();
 
    Node* parent = parentNode();
    ASSERT(parent);

    if (!parent || !parent->isWMLElement())
        return;

    if (static_cast<WMLElement*>(parent)->isWMLTaskElement())
        static_cast<WMLTaskElement*>(parent)->registerVariableSetter(this);
}

String WMLSetvarElement::name() const
{
    return m_name;
}

String WMLSetvarElement::value() const
{
    return m_value;
}

}

#endif