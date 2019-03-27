
//
// SpacialHash.h - fast spacial hash for nearest-neighbor type queries
//

// Copyright (c) 2013-2016 Arthur Danskin
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef SPACIALHASH_H
#define SPACIALHASH_H

template <typename T>
class spatial_hash {

public:

    struct key_type {
        float2       pos;
        float        radius;
        mutable uint query;

        key_type(float2 p, float r) : pos(p), radius(r), query(0) {}
    };

    typedef T                                mapped_type;
    typedef std::pair<key_type, mapped_type> value_type;
    
private:
    //typedef std::vector< uint > bucket_type;
    typedef std::basic_string<uint> bucket_type;

    std::vector<value_type>  m_elements;   // store actual contents
    std::vector<bucket_type> m_cells;      // array of grid cells
    float                    m_cell_size;  // size of each grid cell (width == height)
    float                    m_icell_size; // inverse of m_cell_size
    uint                     m_width;      // sqrt(m_cells.size())
    mutable uint             m_currentQuery;
    
    // return x, y grid cell for position
    int2 scale(float2 p) const
    {
        return int2(floor_int(p.x * m_icell_size), floor_int(p.y * m_icell_size));
    }

    // return grid index for x, y
    uint hash(int2 p) const
    {
        return (p.y * m_width + p.x) % m_cells.size();
    }

    bool acceptElement() const
    {
        return (m_cell_size > 0);
    }

public:

    size_t getSizeof() const
    {
        size_t sz = sizeof(*this);
        sz += SIZEOF_VEC(m_elements);
        sz += SIZEOF_VEC(m_cells);
        return sz;
    }

    const std::vector<value_type> &getElements() const { return m_elements; }

    // change size of hash
    void reset(float cell_size, uint cells)
    {
        clear();
        m_cells.resize(cells);
        m_width      = std::floor(std::sqrt((float)m_cells.size()));
        m_cell_size  = cell_size;
        m_icell_size = 1.f / cell_size;
    }

    // remove all elements from hash
    void clear()
    {
        if (m_elements.size())
        {
            foreach (bucket_type& el, m_cells)
                el.clear();
            m_elements.clear();
        }
        m_currentQuery = 0;
    }

    void shrink_to_fit()
    {
        m_elements.shrink_to_fit();
        m_cells.shrink_to_fit();
        for_ (el, m_cells)
            el.shrink_to_fit();
    }

    int   width()     const { return m_width; }
    float cell_size() const { return m_cell_size; }
    int   cell_count() const { return m_cells.size(); }
    int   elements()  const { return m_elements.size(); }

    spatial_hash(float cell_size, uint cells) { reset(cell_size, cells); }
    spatial_hash() : m_cell_size(0) { }

    void insertPoint(float2 p, const T& v)
    {
        ASSERT(acceptElement());
        if (!acceptElement())
            return;
        const int cell = hash(scale(p));
        m_elements.push_back(make_pair(key_type(p, 0.f), v));
        m_cells[cell].push_back(m_elements.size()-1);
    }
    
    void insertCircle(float2 p, float r, const T& v)
    {
        ASSERT(acceptElement());
        if (!acceptElement())
            return;
        const int2 s = scale(p - float2(r));
        const int2 e = scale(p + float2(r));
        m_elements.push_back(make_pair(key_type(p, r), v));
        for (int x=s.x; x<=e.x; x++)
        {
            for (int y=s.y; y<=e.y; y++)
            {
                // FIXME this is really inserting a square
                const uint   bi = hash(int2(x, y));
                bucket_type &bu  = m_cells[bi];
                bu.push_back(m_elements.size()-1);
            }
        }
    }

    template <typename Fun>
    bool intersectPointEach(float2 p, const Fun& fun) const
    {
        ASSERT(m_cell_size > 0.f);
        if (m_elements.empty())
            return 0;

        const int2         coord  = scale(p);
        const uint         cell   = hash(coord);
        const bucket_type &bucket = m_cells[cell];

        bool foundAny = false;
        m_currentQuery++;

        foreach (const uint idx, bucket)
        {
            const value_type &el = m_elements[idx];
            if (el.first.query != m_currentQuery &&
                intersectPointCircle(p, el.first.pos, el.first.radius))
            {
                el.first.query = m_currentQuery;
                foundAny  = true;
                if (fun(el))
                    return true;
            }
        }
        
        return foundAny;
    }

    template <typename Fun>
    bool intersectCircleEach(float2 p, float r, const Fun& fun) const
    {
        ASSERT(m_cell_size > 0.f);
        if (m_elements.empty())
            return 0;
        
        const int2 s = scale(p - float2(r));
        const int2 e = scale(p + float2(r));

        bool foundAny = false;

        // if we are going to search the whole table, might as well do it efficiently...
        const size_t cellsToSearch = (e.x - s.x) * (e.y - s.y);
        if (cellsToSearch >= m_cells.size())
        {
            foreach (const value_type &el, m_elements) {
                if (intersectCircleCircle(el.first.pos, el.first.radius, p, r)) 
                {
                    foundAny = true;
                    if (fun(el))
                        return true;
                }
            }
            return foundAny;
        }

        const uint query = ++m_currentQuery;

        for (int x=s.x; x<=e.x; x++) {
            for (int y=s.y; y<=e.y; y++)
            {
                const uint         cell   = hash(int2(x, y));
                const bucket_type &bucket = m_cells[cell];
                foreach (const uint idx, bucket)
                {
                    const value_type& el = m_elements[idx];
                    if (el.first.query != query &&
                        intersectCircleCircle(el.first.pos, el.first.radius, p, r))
                    {
                        foundAny = true;
                        el.first.query = query;
                        if (fun(el))
                            return true;
                    }
                }
            }
        }
        
        return foundAny;
    }


    template <typename Fun>
    bool intersectRectangleEach(float2 p, float2 r, const Fun& fun) const
    {
        ASSERT(m_cell_size > 0.f);
        if (m_elements.empty())
            return 0;
        
        const int2 s = scale(p - r);
        const int2 e = scale(p + r);

        bool foundAny = false;
        
        // if we are going to search the whole table, might as well do it efficiently...
        const size_t cellsToSearch = (e.x - s.x) * (e.y - s.y);
        if (cellsToSearch >= m_cells.size())
        {
            foreach (const value_type &el, m_elements) {
                if (intersectCircleRectangle(el.first.pos, el.first.radius, p, r)) 
                {
                    foundAny = true;
                    if (fun(el))
                        return true;
                }
            }
            return foundAny;
        }

        const uint query = ++m_currentQuery;

        for (int x=s.x; x<=e.x; x++) {
            for (int y=s.y; y<=e.y; y++)
            {
                const uint         cell   = hash(int2(x, y));
                const bucket_type &bucket = m_cells[cell];
                foreach (const uint idx, bucket)
                {
                    const value_type &el = m_elements[idx];
                    if (el.first.query != query &&
                        intersectCircleRectangle(el.first.pos, el.first.radius, p, r))
                    {
                        el.first.query = query;
                        foundAny  = true;
                        if (fun(el))
                            return true;
                    }
                }
            }
        }
        
        return foundAny;
    }

    template <typename Fun>
    bool each(const Fun& fun) const
    {
        ASSERT(m_cell_size > 0.f);
        if (m_elements.size() == 0)
            return false;
        
        foreach (const value_type &el, m_elements) {
            if (fun(el))
                return true;
        }
        return true;
    }


    // add all elements within the input circle to the input vector
    // return count of items found
    int intersectCircle(vector<T>* output, float2 p, float r) const
    {
        int count = 0;
        intersectCircleEach(p, r, [&](const value_type &val)
                            { 
                                output->push_back(const_cast<T&>(val.second));
                                count++;
                                return false;
                            });
        return count;
    }


    struct QueryNearest {

        const float2              center;
        mutable float             nearestDist = std::numeric_limits<float>::max();
        mutable const value_type *nearestElt;

        QueryNearest(const value_type *def) : center(def->first.pos), nearestElt(def) {}

        bool operator()(const value_type& el) const
        {
            const float distSqr = distanceSqr(el.first.pos, center);
            if (distSqr < squared(nearestDist + el.first.radius))
            {
                nearestDist = std::sqrt(distSqr) - el.first.radius;
                nearestElt  = &el;
            }
            return true;
        }
    };

    // return item nearest to p within radius r
    value_type intersectCircleNearest(float2 p, float r, const T& def=T()) const
    {
        value_type qval = make_pair(key_type(p, r), def);
        QueryNearest query(&qval);
        
        intersectCircleEach(p, r, query);
        return *query.nearestElt;
    }

    value_type intersectPointNearest(float2 p, const T& def=T()) const
    {
        value_type qval = make_pair(key_type(p, 0.f), def);
        QueryNearest query(&qval);
        
        intersectPointEach(p, query);
        return *query.nearestElt;
    }

    bool intersectCircle(float2 p, float r) const
    {
        return intersectCircleEach(p, r, [&](const value_type& el) { return false; });
    }

    
};


#endif // SPACIALHASH_H
