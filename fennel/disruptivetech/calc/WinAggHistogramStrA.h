/*
// $Id$
// Fennel is a library of data storage and processing components.
// Copyright (C) 2006-2006 Disruptive Tech
// Copyright (C) 2006-2006 The Eigenbase Project
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
//
// WinAggHistogram - Windowed Aggregation Histogram object.
*/

#ifndef Fennel_WinAggHistogramStrA_Included
#define Fennel_WinAggHistogramStrA_Included

#include "fennel/tuple/TupleDescriptor.h"
#include "fennel/disruptivetech/calc/CalcCommon.h"
#include "fennel/disruptivetech/calc/SqlString.h"
#include "fennel/common/TraceSource.h"

#include <utility>
#include <set>

FENNEL_BEGIN_NAMESPACE

/**
 * Support structure for calculating various windowed aggregation
 * functions (COUNT, SUM, AVG, MIN, MAX, FIRST_VALUE, LAST_VALUE)
 * against character data.
 *
 * Each row entry is held in
 * a tree structure to make finding new min/max functions easy as values
 * are added/removed from the window.  Running sum is also kept up to date
 * as rows enter and exit the window.
 *
 * It is provided as a parameter to all windowed agg support functions.
 *
 * @author JFrost
 * @version $Id$
 */

struct StringDesc : public TupleDatum
{
    TupleStorageByteLength cbStorage;
    StandardTypeDescriptorOrdinal mType;

    StringDesc(){};
    ~StringDesc(){};

    StringDesc(StringDesc const &other)
    {
        copyFrom(other);
    }

    StringDesc& operator = (StringDesc const &other)
    {
        copyFrom(other);
        return *this;
    }
    

    void copyFrom(StringDesc const &other)
    {
        TupleDatum::copyFrom( other);
        cbStorage = other.cbStorage;
        mType = other.mType;
    }

    char* pointer() const;
    TupleStorageByteLength stringLength() const;

};

class WinAggHistogramStrA
{

public:
    WinAggHistogramStrA()
        : currentWindow(),
          nullRows(0),
          queue()
    {}
                       
    ~WinAggHistogramStrA()
    {}

    typedef struct _StringDescCompare
    {
        bool operator () (const StringDesc& str1, const StringDesc& str2) const
        {
            if (!str1.isNull() && !str2.isNull()) {
                int32_t result = SqlStrCmp<1,1>(
                    str1.pointer(), str1.stringLength(),
                    str2.pointer(), str2.stringLength());
                return result < 0;
            } else {
                return str2.isNull();
            }
        }
    } StringDescCompare;

    // REVIEW: jhyde, 2006/6/14: Use initcaps for type names.
    typedef multiset<StringDesc,StringDescCompare> winAggData;

    typedef deque<StringDesc> WinAggQueue;

    //! addRow - Adds new value to tree and updates
    //! the running sum for current values.
    //!
    //! Input - New value to be added to the tree
    //
    void addRow(RegisterRef<char*>* node);
    
    //! dropRow - Removes a value from the tree and updates
    //! the running sum..
    //!
    //! Input - Value to be removed from the tree
    //
    void dropRow(RegisterRef<char*>* node);

    //! getMin - Returns the current MIN() value for the window.
    //!
    //! Returns NULL if the window is empty.
    void getMin(RegisterRef<char*>* node);

    //! getMax - Returns the current MAX() value for the window.
    //!
    //! Returns NULL if the window is empty.
    void getMax(RegisterRef<char*>* node);

    //! getFirstValue - returns the first value which entered the tree
    //!
    //! Returns NULL if the window is empty.
    void getFirstValue(RegisterRef<char*>* node);

    //! getLastValue - returns the last value which entered the tree
    //!
    //! Returns NULL if the window is empty.
    void getLastValue(RegisterRef<char*>* node);

private:
    winAggData currentWindow;   // Holds the values currently in the window.
    int64_t nullRows;           // Couunt of null entries

    /// FIFO queue of values, to enable FIRST_VALUE/LAST_VALUE support.
    WinAggQueue queue;
};

FENNEL_END_NAMESPACE

#endif

// End WinAggAccum.h