/*
// $Id$
// Farrago is an extensible data management system.
// Copyright (C) 2005-2005 The Eigenbase Project
// Copyright (C) 2005-2005 Disruptive Tech
// Copyright (C) 2005-2005 LucidEra, Inc.
// Portions Copyright (C) 2003-2005 John V. Sichi
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your option)
// any later version approved by The Eigenbase Project.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
package net.sf.farrago.jdbc.engine;

import net.sf.farrago.resource.FarragoResource;

import org.eigenbase.reltype.RelDataType;
import org.eigenbase.util.Util;

/**
 * FarragoJdbcEngineBinaryParamDef defines a binary parameter. Only accepts 
 * byte-array values.
 * 
 * @author Julian Hyde
 * @version $Id$
 */
class FarragoJdbcEngineBinaryParamDef extends FarragoJdbcEngineParamDef
{
    private final int maxByteCount;

    public FarragoJdbcEngineBinaryParamDef(
        String paramName,
        RelDataType type)
    {
        super(paramName, type);
        maxByteCount = type.getPrecision();
    }

    // implement FarragoSessionStmtParamDef
    public Object scrubValue(Object x)
    {
        if (x == null) {
            return x;
        }
        if (!(x instanceof byte [])) {
            throw newInvalidType(x);
        }
        final byte [] bytes = (byte []) x;
        if (bytes.length > maxByteCount) {
            throw FarragoResource.instance().ParameterValueTooLong.ex(
                Util.toStringFromByteArray(bytes,16),
                type.toString());
        }
        return bytes;
    }
}