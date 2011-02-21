/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is
 * 'psimpl - generic n-dimensional polyline simplification'.
 *
 * The Initial Developer of the Original Code is
 * Elmar de Koning.
 * Portions created by the Initial Developer are Copyright (C) 2010
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * ***** END LICENSE BLOCK ***** */

/*!
    psimpl - generic n-dimensional polyline simplification
    Copyright (C) 2010 Elmar de Koning, edekoning@gmail.com

    This file is part of psimpl and was originally released as part
    of the article 'Polyline Simplification' at The Code Project:
    http://www.codeproject.com/KB/recipes/PolylineSimplification.aspx

    28-09-2010 - Initial version
    23-10-2010 - Changed license from CPOL to MPL
    26-10-2010 - Clarified input (type) requirements, and changed the
                 behavior of the algorithms under invalid input
    01-12-2010 - Added the nth point, perpendicular distance and Reumann-Witkam routines;
                 moved all functions related to distance calculations to the math namespace
*/

#ifndef PSIMPL_GENERIC
#define PSIMPL_GENERIC


#include <queue>
#include <stack>


/*!
    \brief Root namespace of the polyline simplification library.
*/
namespace psimpl
{
    /*!
        \brief A smart pointer for holding a dynamically allocated array.

        Similar to boost::scoped_array.
    */
    template <typename T>
    class scoped_array
    {
    public:
        scoped_array (unsigned n) {
            array = new T [n];
        }

        ~scoped_array () {
            delete [] array;
        }

        T& operator [] (int offset) {
            return array [offset];
        }

        const T& operator [] (int offset) const {
            return array [offset];
        }

        T* get () const {
            return array;
        }

        void swap (scoped_array& b) {
            T* tmp = b.array;
            b.array = array;
            array = tmp;
        }

    private:
        scoped_array (const scoped_array&);
        scoped_array& operator= (const scoped_array&);

    private:
        T* array;
    };

    template <class T> inline void swap (scoped_array <T>& a, scoped_array <T>& b) {
        a.swap (b);
    }

    /*!
        \brief Contains functions for calculating distances between various geometric entities.
    */
    namespace math
    {
        /*!
            \brief Creates a vector from two points.

            \param[in] p1       the first coordinate of the first point
            \param[in] p2       the first coordinate of the second point
            \param[out] vector  the resulting vector (p2-p1)
            \return             one beyond the last coordinate of the resulting vector
        */
        template <unsigned DIM, class InputIterator, class OutputIterator>
        inline OutputIterator make_vector (
            InputIterator p1,
            InputIterator p2,
            OutputIterator result)
        {
            for (unsigned d = 0; d < DIM; ++d, ++p1, ++p2) {
                *result++ = *p2 - *p1;
            }
            return result;
        }

        /*!
            \brief Computes the dot product of two vectors.

            \param[in] v1   the first coordinate of the first vector
            \param[in] v2   the first coordinate of the second vector
            \return         the dot product (v1 * v2)
        */
        template <unsigned DIM, class InputIterator>
        inline typename std::iterator_traits <InputIterator>::value_type dot (
            InputIterator v1,
            InputIterator v2)
        {
            typename std::iterator_traits <InputIterator>::value_type result = 0;
            for (unsigned d = 0; d < DIM; ++d, ++v1, ++v2) {
                result += (*v1) * (*v2);
            }
            return result;
        }

        /*!
            \brief Peforms linear interpolation between two points.

            \param[in] p1           the first coordinate of the first point
            \param[in] p2           the first coordinate of the second point
            \param[in] fraction     the fraction used during interpolation
            \param[out] result      the interpolation result (p1 + fraction * (p2 - p1))
            \return                 one beyond the last coordinate of the interpolated point
        */
        template <unsigned DIM, class InputIterator, class OutputIterator>
        inline OutputIterator interpolate (
            InputIterator p1,
            InputIterator p2,
            float fraction,
            OutputIterator result)
        {
            for (unsigned d = 0; d < DIM; ++d, ++p1, ++p2) {
                *result++ = *p1 + fraction * (*p2 - *p1);
            }
            return result;
        }

        /*!
            \brief Computes the squared distance of two points

            \param[in] p1   the first coordinate of the first point
            \param[in] p2   the first coordinate of the second point
            \return         the squared distance
        */
        template <unsigned DIM, class InputIterator1, class InputIterator2>
        inline typename std::iterator_traits <InputIterator1>::value_type point_distance2 (
            InputIterator1 p1,
            InputIterator2 p2)
        {
            typename std::iterator_traits <InputIterator1>::value_type result = 0;
            for (unsigned d = 0; d < DIM; ++d, ++p1, ++p2) {
                result += (*p1 - *p2) * (*p1 - *p2);
            }
            return result;
        }

        /*!
            \brief Computes the squared distance between an infinite line (l1, l2) and a point p

            \param[in] l1   the first coordinate of the first point on the line
            \param[in] l2   the first coordinate of the second point on the line
            \param[in] p    the first coordinate of the test point
            \return         the squared distance
        */
        template <unsigned DIM, class InputIterator>
        inline typename std::iterator_traits <InputIterator>::value_type line_distance2 (
            InputIterator l1,
            InputIterator l2,
            InputIterator p)
        {
            typedef typename std::iterator_traits <InputIterator>::value_type value_type;

            value_type v [DIM];                    // vector l1 --> l2
            value_type w [DIM];                    // vector l1 --> p

            make_vector <DIM> (l1, l2, v);
            make_vector <DIM> (l1, p,  w);

            value_type cv = dot <DIM> (v, v);    // squared length of v
            value_type cw = dot <DIM> (w, v);    // project w onto v

            // avoid problems with divisions when value_type is an integer type
            float fraction = static_cast <float> (cw) / static_cast <float> (cv);

            value_type proj [DIM];    // p projected onto line (l1, l2)
            interpolate <DIM> (l1, l2, fraction, proj);

            return point_distance2 <DIM> (p, proj);
        }

        /*!
            \brief Computes the squared distance between a line segment (s1, s2) and a point p

            \param[in] s1   the first coordinate of the start point of the segment
            \param[in] s2   the first coordinate of the end point of the segment
            \param[in] p    the first coordinate of the test point
            \return         the squared distance
        */
        template <unsigned DIM, class InputIterator>
        inline typename std::iterator_traits <InputIterator>::value_type segment_distance2 (
            InputIterator s1,
            InputIterator s2,
            InputIterator p)
        {
            typedef typename std::iterator_traits <InputIterator>::value_type value_type;

            value_type v [DIM];        // vector s1 --> s2
            value_type w [DIM];        // vector s1 --> p

            make_vector <DIM> (s1, s2, v);
            make_vector <DIM> (s1, p,  w);

            value_type cw = dot <DIM> (w, v);    // project w onto v
            if (cw <= 0) {
                // projection of w lies to the left of s1
                return point_distance2 <DIM> (p, s1);
            }

            value_type cv = dot <DIM> (v, v);    // squared length of v
            if (cv <= cw) {
                // projection of w lies to the right of s2
                return point_distance2 <DIM> (p, s2);
            }

            // avoid problems with divisions when value_type is an integer type
            float fraction = static_cast <float> (cw) / static_cast <float> (cv);

            value_type proj [DIM];    // p projected onto segement (s1, s2)
            interpolate <DIM> (s1, s2, fraction, proj);

            return point_distance2 <DIM> (p, proj);
        }
    };

    /*!
        \brief Provides various simplification algorithms for n-dimensional simple polylines.

        A polyline is simple when it is non-closed and non-selfintersecting. All algorithms
        operate on input iterators and output iterators. Note that unisgned integer types are
        NOT supported.
    */
    template <unsigned DIM, class InputIterator, class OutputIterator>
    class PolylineSimplification
    {
        typedef typename std::iterator_traits <InputIterator>::difference_type diff_type;
        typedef typename std::iterator_traits <InputIterator>::value_type value_type;
        typedef typename std::iterator_traits <const value_type*>::difference_type ptr_diff_type;

    public:
        /*!
            \brief Performs the nth point routine (NP).

            NP is an O(n) algorithm for polyline simplification. It keeps only the first, last and
            each nth point. As an example, consider any random line of 8 points. Using n = 3 will
            always yield a simplification consisting of points: 1, 4, 7, 8

            NP is applied to the range [first, last). The resulting simplified polyline is copied
            to the output range [result, result + m*DIM), where m is the number of vertices of the
            simplified polyline. The return value is the end of the output range: result + m*DIM.

            Input (Type) requirements:
            1- DIM is not zero, where DIM represents the dimension of the polyline.
            2- The InputIterator value type is convertible to a value type of the output iterator.
            3- The range [first, last) contains only vertex coordinates in multiples of DIM, f.e.:
               x, y, z, x, y, z, x, y, z when DIM = 3.
            4- The range [first, last) contains at least 2 vertices.
            5- n is at least 2.

            In case these requirements are not met, compile errors may occur OR the entire input
            range [first, last) is copied to the output range [result, result + (last - first))

            \param[in] first    the first coordinate of the first polyline point
            \param[in] last     one beyond the last coordinate of the last polyline point
            \param[in] n        specifies 'each nth point'
            \param[out] result  destination of the simplified polyline
            \return             one beyond the last coordinate of the simplified polyline
        */
        OutputIterator NthPoint (
            InputIterator first,
            InputIterator last,
            unsigned n,
            OutputIterator result)
        {
            diff_type coordCount = std::distance (first, last);
            diff_type pointCount = DIM      // protect against zero DIM
                                   ? coordCount / DIM
                                   : 0;

            // validate input
            if (coordCount % DIM || pointCount < 2 || n < 2) {
                std::copy (first, last, result);
                return result;
            }

            InputIterator key = first;

            // the first point is always part of the simplification
            CopyKey (key, result);

            unsigned k = (pointCount-1) / n;    // number of nth points after first
            unsigned r = pointCount - k*n - 1;  // number of points between the final nth point and last

            // add the first point and the following k nth points to the simplification
            for (unsigned i=0; i<k; ++i) {
                std::advance (key, n*DIM);
                CopyKey (key, result);
            }
            // make sure the last point is part of the simplification
            if (r) {
                std::advance (key, DIM * r);
                CopyKey (key, result);
            }
            return result;
        }

        /*!
            \brief Performs the (radial) distance between points routine (RD).

            RD is a brute-force O(n) algorithm for polyline simplification. It reduces successive
            vertices that are clustered too closely to a single vertex, called a key. The resulting
            keys form the simplified polyline.

            RD is applied to the range [first, last) using the specified tolerance tol. The
            resulting simplified polyline is copied to the output range [result, result + m*DIM),
            where m is the number of vertices of the simplified polyline. The return value is the
            end of the output range: result + m*DIM.

            Input (Type) requirements:
            1- DIM is not zero, where DIM represents the dimension of the polyline.
            2- The InputIterator value type is convertible to a value type of the output iterator.
            3- The range [first, last) contains only vertex coordinates in multiples of DIM, f.e.:
               x, y, z, x, y, z, x, y, z when DIM = 3.
            4- The range [first, last) contains at least 2 vertices.
            5- tol is not zero.

            In case these requirements are not met, compile errors may occur OR the entire input
            range [first, last) is copied to the output range [result, result + (last - first))

            \param[in] first    the first coordinate of the first polyline point
            \param[in] last     one beyond the last coordinate of the last polyline point
            \param[in] tol      radial (point-to-point) distance tolerance
            \param[out] result  destination of the simplified polyline
            \return             one beyond the last coordinate of the simplified polyline
        */
        OutputIterator RadialDistance (
            InputIterator first,
            InputIterator last,
            value_type tol,
            OutputIterator result)
        {
            diff_type coordCount = std::distance (first, last);
            diff_type pointCount = DIM      // protect against zero DIM
                                   ? coordCount / DIM
                                   : 0;
            value_type tol2 = tol * tol;    // squared distance tolerance

            // validate input
            if (coordCount % DIM || pointCount < 2 || tol2 == 0) {
                std::copy (first, last, result);
                return result;
            }

            InputIterator current = first;  // indicates the current key
            InputIterator next = first;     // used to find the next key

            // the first point is always part of the simplification
            CopyKey (current, result);

            // Skip first and last point, because they are always part of the simplification
            for (diff_type index = 1; index < pointCount - 1; ++index) {
                std::advance (next, DIM);
                if (math::point_distance2 <DIM> (current, next) < tol2) {
                    continue;
                }
                current = next;
                CopyKey (current, result);
            }
            // the last point is always part of the simplification
            std::advance (next, DIM);
            CopyKey (next, result);

            return result;
        }

        /*!
            \brief Repeatedly performs the perpendicular distance routine (PD).

            The algorithm stops after calling the PD routine 'repeat' times OR when the
            simplification does not improve. Note that this algorithm will need to store
            up to two intermediate simplification results.

            \param[in] first    the first coordinate of the first polyline point
            \param[in] last     one beyond the last coordinate of the last polyline point
            \param[in] tol      perpendicular (segment-to-point) distance tolerance
            \param[in] repeat   the number of times to successively apply the PD routine.
            \param[out] result  destination of the simplified polyline
            \return             one beyond the last coordinate of the simplified polyline
        */
        OutputIterator PerpendicularDistance (
            InputIterator first,
            InputIterator last,
            value_type tol,
            unsigned repeat,
            OutputIterator result)
        {
            if (repeat == 1) {
                // single pass
                return PerpendicularDistance (first, last, tol, result);
            }
            // only validate repeat; other input is validated by simplify_perpendicular_distance
            if (repeat < 1) {
                std::copy (first, last, result);
                return result;
            }
            diff_type coordCount = std::distance (first, last);

            // first pass: [first, last) --> temporary array 'tempPoly'
            scoped_array <value_type> tempPoly (coordCount);
            PolylineSimplification <DIM, InputIterator, value_type*> psimpl_to_array;
            diff_type tempCoordCount = std::distance (tempPoly.get (),
                psimpl_to_array.PerpendicularDistance (first, last, tol, tempPoly.get ()));

            // check if simplification did not improved
            if (coordCount == tempCoordCount) {
                std::copy (tempPoly.get (), tempPoly.get () + coordCount, result);
                return result;
            }
            std::swap (coordCount, tempCoordCount);
            --repeat;

            // intermediate passes: temporary array 'tempPoly' --> temporary array 'tempResult'
            if (1 < repeat) {
                scoped_array <value_type> tempResult (coordCount);
                PolylineSimplification <DIM, value_type*, value_type*> psimpl_arrays;

                while (--repeat) {
                    tempCoordCount = std::distance (tempResult.get (),
                        psimpl_arrays.PerpendicularDistance (
                            tempPoly.get (), tempPoly.get () + coordCount, tol, tempResult.get ()));

                    // check if simplification did not improved
                    if (coordCount == tempCoordCount) {
                        std::copy (tempPoly.get (), tempPoly.get () + coordCount, result);
                        return result;
                    }
                    swap (tempPoly, tempResult);
                    std::swap (coordCount, tempCoordCount);
                }
            }

            // final pass: temporary array 'tempPoly' --> result
            PolylineSimplification <DIM, value_type*, OutputIterator> psimpl_from_array;
            return psimpl_from_array.PerpendicularDistance (
                tempPoly.get (), tempPoly.get () + coordCount, tol, result);
        }

        /*!
            \brief Performs the perpendicular distance routine (PD).

            PD is an O(n) algorithm for polyline simplification. It computes the perpendicular
            distance of each point pi to the line segment S(pi-1, pi+1). Only when this distance is
            larger than the given tolerance will pi be part of the simpification. Note that the
            original polyline can only be reduced by a maximum of 50%. Multiple passes are required
            to achieve higher points reductions.

            PD is applied to the range [first, last) using the specified tolerance tol. The
            resulting simplified polyline is copied to the output range [result, result + m*DIM),
            where m is the number of vertices of the simplified polyline. The return value is the
            end of the output range: result + m*DIM.

            Input (Type) requirements:
            1- DIM is not zero, where DIM represents the dimension of the polyline.
            2- The InputIterator value type is convertible to a value type of the output iterator.
            3- The range [first, last) contains only vertex coordinates in multiples of DIM, f.e.:
               x, y, z, x, y, z, x, y, z when DIM = 3.
            4- The range [first, last) contains at least 2 vertices.
            5- tol is not zero.

            In case these requirements are not met, compile errors may occur OR the entire input
            range [first, last) is copied to the output range [result, result + (last - first))

            \param[in] first    the first coordinate of the first polyline point
            \param[in] last     one beyond the last coordinate of the last polyline point
            \param[in] tol      perpendicular (segment-to-point) distance tolerance
            \param[out] result  destination of the simplified polyline
            \return             one beyond the last coordinate of the simplified polyline
        */
        OutputIterator PerpendicularDistance (
            InputIterator first,
            InputIterator last,
            value_type tol,
            OutputIterator result)
        {
            diff_type coordCount = std::distance (first, last);
            diff_type pointCount = DIM      // protect against zero DIM
                                   ? coordCount / DIM
                                   : 0;
            value_type tol2 = tol * tol;    // squared distance tolerance

            // validate input
            if (coordCount % DIM || pointCount < 3 || tol2 == 0) {
                std::copy (first, last, result);
                return result;
            }

            InputIterator p0 = first;
            InputIterator p1 = p0;
            std::advance (p1, DIM);
            InputIterator p2 = p1;
            std::advance (p2, DIM);

            // the first point is always part of the simplification
            CopyKey (p0, result);

            while (p1 != last && p2 != last) {
                // test p1 against line segment S(p0, p2)
                if (math::segment_distance2 <DIM> (p0, p2, p1) < tol2) {
                    CopyKey (p2, result);
                    // move up by two points
                    p0 = p2;
                    std::advance (p1, 2*DIM);
                    std::advance (p2, 2*DIM);
                }
                else {
                    CopyKey (p1, result);
                    // move up by one point
                    p0 = p1;
                    p1 = p2;
                    std::advance (p2, DIM);
                }
            }
            // make sure the last point is part of the simplification
            if (p1 != last) {
                CopyKey (p1, result);
            }

            return result;
        }

        /*!
            \brief Performs Reumann-Witkam approximation (RW).

            The O(n) RW routine uses a point-to-line (perpendicular) distance tolerance. It defines
            a line through the first two vertices of the original polyline. For each successive
            vertex vi its perpendicular distance to this line is calculated. A new key is found at
            vi-1, when this distance exceeds the specified tolerance. The vertices vi and vi+1 are
            then used to define a new line, and the process repeats itself.

            RW routine is applied to the range [first, last) using the specified perpendicular
            distance tolerance tol. The resulting simplified polyline is copied to the output range
            [result, result + m*DIM), where m is the number of vertices of the simplified polyline.
            The return value is the end of the output range: result + m*DIM.

            Input (Type) Requirements:
            1- DIM is not zero, where DIM represents the dimension of the polyline.
            2- The InputIterator value type is convertible to a value type of the output iterator.
            3- The range [first, last) contains vertex coordinates in multiples of DIM,
               f.e.: x, y, z, x, y, z, x, y, z when DIM = 3.
            4- The range [first, last) contains at least three vertices.
            5- tol is not zero.

            In case these requirements are not met, compile errors may occur OR the entire input
            range [first, last) is copied to the output range [result, result + (last - first))

            \param[in] first    the first coordinate of the first polyline point
            \param[in] last     one beyond the last coordinate of the last polyline point
            \param[in] tol      perpendicular (point-to-line) distance tolerance
            \param[out] result  destination of the simplified polyline
            \return             one beyond the last coordinate of the simplified polyline
        */
        OutputIterator ReumannWitkam (
            InputIterator first,
            InputIterator last,
            value_type tol,
            OutputIterator result)
        {
            diff_type coordCount = std::distance (first, last);
            diff_type pointCount = DIM      // protect against zero DIM
                                   ? coordCount / DIM
                                   : 0;
            value_type tol2 = tol * tol;    // squared distance tolerance

            // validate input
            if (coordCount % DIM || pointCount < 3 || tol2 == 0) {
                std::copy (first, last, result);
                return result;
            }

            // define the line L(p0, p1)
            InputIterator p0 = first;  // indicates the current key
            InputIterator p1 = first;  // indicates the next point after p0
            std::advance (p1, DIM);

            // keep track of two test points
            InputIterator pi = p1;     // the previous test point
            InputIterator pj = p1;     // the current test point (pi+1)

            // the first point is always part of the simplification
            CopyKey (p0, result);

            // check each point pj against L(p0, p1)
            for (diff_type j = 2; j < pointCount; ++j) {
                pi = pj;
                std::advance (pj, DIM);

                if (math::line_distance2 <DIM> (p0, p1, pj) < tol2) {
                    continue;
                }
                // found the next key at pi
                CopyKey (pi, result);
                // define new line L(pi, pj)
                p0 = pi;
                p1 = pj;
            }
            // the last point is always part of the simplification
            CopyKey (pj, result);

            return result;
        }

        /*!
            \brief Performs Douglas-Peucker approximation (DP).

            The DP algorithm uses the RadialDistance (RD) routine O(n) as a preprocessing step.
            After RD the algorithm is O (n m) in worst case and O(n log m) on average, where m < n
            (m is the number of points after RD).

            The DP algorithm starts with a simplification that is the single edge joining the first
            and last vertices of the polyline. The distance of the remaining vertices to that edge
            are computed. The vertex that is furthest away from theedge (called a key), and has a
            computed distance that is larger than a specified tolerance, will be added to the
            simplification. This process will recurse for each edge in the current simplification,
            untill all vertices of the original polyline are within tolerance.

            Note that this algorithm will create a copy of the input polyline during the vertex
            reduction step.

            RD followed by DP is applied to the range [first, last) using the specified tolerance
            tol. The resulting simplified polyline is copied to the output range
            [result, result + m*DIM), where m is the number of vertices of the simplified polyline.
            The return value is the end of the output range: result + m*DIM.

            Input (Type) requirements:
            1- DIM is not zero, where DIM represents the dimension of the polyline.
            2- The InputIterator value type is convertible to a value type of the output iterator.
            3- The range [first, last) contains vertex coordinates in multiples of DIM, f.e.:
               x, y, z, x, y, z, x, y, z when DIM = 3.
            4- The range [first, last) contains at least 2 vertices.
            5- tol is not zero.

            In case these requirements are not met, compile errors may occur OR the entire input
            range [first, last) is copied to the output range [result, result + (last - first))

            \param[in] first    the first coordinate of the first polyline point
            \param[in] last     one beyond the last coordinate of the last polyline point
            \param[in] tol      perpendicular (point-to-segment) distance tolerance
            \param[out] result  destination of the simplified polyline
            \return             one beyond the last coordinate of the simplified polyline
        */
        OutputIterator DouglasPeucker (
            InputIterator first,
            InputIterator last,
            value_type tol,
            OutputIterator result)
        {
            diff_type coordCount = std::distance (first, last);
            diff_type pointCount = DIM      // protect against zero DIM
                                   ? coordCount / DIM
                                   : 0;
            // validate input
            if (coordCount % DIM || pointCount < 2 || tol == 0) {
                std::copy (first, last, result);
                return result;
            }
            // (radial) distance between points routine
            scoped_array <value_type> reduced (coordCount);     // radial distance results
            PolylineSimplification <DIM, InputIterator, value_type*> psimpl_to_array;
            ptr_diff_type reducedCoordCount = std::distance (reduced.get (),
                psimpl_to_array.RadialDistance (first, last, tol, reduced.get ()));
            ptr_diff_type reducedPointCount = reducedCoordCount / DIM;

            // douglas-peucker approximation
            scoped_array <unsigned char> keys (pointCount);     // douglas-peucker results
            DPHelper::Approximate (reduced.get (), reducedCoordCount, tol, keys.get ());

            // copy all keys
            const value_type* current = reduced.get ();
            for (ptr_diff_type p=0; p<reducedPointCount; ++p, current += DIM) {
                if (keys [p]) {
                    for (unsigned d = 0; d < DIM; ++d) {
                        *result++ = current [d];
                    }
                }
            }
            return result;
        }

        /*!
            \brief Performs a Douglas-Peucker approximation variant (DPa).

            This algorithm is a variation of the original implementation. Instead of considering
            one polyline segment at a time, all segments of the current simplified polyline are
            evaluated at each step. Only the vertex with the maximum distance from its edge is
            added to the simplification. This process will recurse untill the the simplification
            contains the desired amount of vertices.

            The algorithm, which does not use the (radial) distance between points routine as a
            preprocessing step, is O(n2) in worst case and O(n log n) on average.

            Note that this algorithm will create a copy of the input polyline for performance
            reasons.

            DPa is applied to the range [first, last). The resulting simplified polyline consists
            of count vertices and is copied to the output range [result, result + count). The
            return value is the end of the output range: result + count.

            Input (Type) requirements:
            1- DIM is not zero, where DIM represents the dimension of the polyline.
            2- The InputIterator value type is convertible to a value type of the output iterator.
            3- The range [first, last) contains vertex coordinates in multiples of DIM, f.e.:
               x, y, z, x, y, z, x, y, z when DIM = 3.
            4- The range [first, last) contains a minimum of count vertices.
            5- count is at least 2

            In case these requirements are not met, compile errors may occur OR the entire input
            range [first, last) is copied to the output range [result, result + (last - first))

            \param[in] first    the first coordinate of the first polyline point
            \param[in] last     one beyond the last coordinate of the last polyline point
            \param[in] count    the maximum number of points of the simplified polyline
            \param[out] result  destination of the simplified polyline
            \return             one beyond the last coordinate of the simplified polyline
        */
        OutputIterator DouglasPeuckerAlt (
            InputIterator first,
            InputIterator last,
            unsigned count,
            OutputIterator result)
        {
            diff_type coordCount = std::distance (first, last);
            diff_type pointCount = DIM      // protect against zero DIM
                                   ? coordCount / DIM
                                   : 0;
            // validate input
            if (coordCount % DIM || pointCount <= static_cast <diff_type> (count) || count < 2) {
                std::copy (first, last, result);
                return result;
            }

            // copy coords
            scoped_array <value_type> coords (coordCount);
            for (ptr_diff_type c=0; c<coordCount; ++c) {
                coords [c] = *first++;
            }

            // douglas-peucker approximation
            scoped_array <unsigned char> keys (pointCount);
            DPHelper::ApproximateAlt (coords.get (), coordCount, count, keys.get ());

            // copy keys
            const value_type* current = coords.get ();
            for (ptr_diff_type p=0; p<pointCount; ++p, current += DIM) {
                if (keys [p]) {
                    for (unsigned d = 0; d < DIM; ++d) {
                        *result++ = current [d];
                    }
                }
            }
            return result;
        }

    private:
        /*!
            \brief Copies a key to the output destination.

            \param[in] key      the first coordinate of the key
            \param[out] result  destination of the copied key
            \return             one beyond beyond the last coordinate of the copied key
        */
        inline OutputIterator CopyKey (
            InputIterator key,
            OutputIterator& result)
        {
            for (unsigned d = 0; d < DIM; ++d) {
                *result++ = *key++;
            }
            return result;
        }

    private:
        /*!
            \brief Douglas-Peucker approximation helper class.

            Contains helper implentations for Douglas-Peucker approximation that operate solely on
            value_type arrays and value_type pointers. Note that the PolylineSimplification
            class only operates on iterators.
        */
        class DPHelper
        {
            //! \brief Defines a sub polyline.
            struct SubPoly {
                SubPoly (ptr_diff_type first=0, ptr_diff_type last=0) :
                    first (first), last (last) {}

                ptr_diff_type first;    //! coord index of the first point
                ptr_diff_type last;     //! coord index of the last point
            };

            //! \brief Defines the key of a polyline.
            struct KeyInfo {
                KeyInfo (ptr_diff_type index=0, value_type dist2=0) :
                    index (index), dist2 (dist2) {}

                ptr_diff_type index;    //! coord index of the key
                value_type dist2;       //! squared distance of the key to a segment
            };

            //! \brief Defines a sub polyline including its key.
            struct SubPolyAlt {
                SubPolyAlt (ptr_diff_type first=0, ptr_diff_type last=0) :
                    first (first), last (last) {}

                ptr_diff_type first;    //! coord index of the first point
                ptr_diff_type last;     //! coord index of the last point
                KeyInfo keyInfo;        //! key of this sub poly

                bool operator< (const SubPolyAlt& other) const {
                    return keyInfo.dist2 < other.keyInfo.dist2;
                }
            };

        public:
            /*!
                \brief Performs Douglas-Peucker approximation.

                \param[in] coords       array of polyline coordinates
                \param[in] coordCount   number of coordinates in coords []
                \param[in] tol          approximation tolerance
                \param[out] keys        indicates for each polyline point if it is a key
            */
            static void Approximate (
                const value_type* coords,
                ptr_diff_type coordCount,
                value_type tol,
                unsigned char* keys)
            {
                value_type tol2 = tol * tol;    // squared distance tolerance
                ptr_diff_type pointCount = coordCount / DIM;
                // zero out keys
                memset (keys, 0, pointCount * sizeof (unsigned char));
                keys [0] = 1;                   // the first point is always a key
                keys [pointCount - 1] = 1;      // the last point is always a key

                typedef std::stack <SubPoly> Stack;
                Stack stack;                    // LIFO job-queue containing sub-polylines

                SubPoly subPoly (0, coordCount-DIM);
                stack.push (subPoly);           // add complete poly

                while (!stack.empty ()) {
                    subPoly = stack.top ();     // take a sub poly
                    stack.pop ();               // and find its key
                    KeyInfo keyInfo = FindKey (coords, subPoly.first, subPoly.last);
                    if (keyInfo.index != subPoly.last && tol2 < keyInfo.dist2) {
                        // store the key if valid
                        keys [keyInfo.index / DIM] = 1;
                        // split the polyline at the key and recurse
                        stack.push (SubPoly (keyInfo.index, subPoly.last));
                        stack.push (SubPoly (subPoly.first, keyInfo.index));
                    }
                }
            }

            /*!
                \brief Performs Douglas-Peucker approximation.

                \param[in] coords       array of polyline coordinates
                \param[in] coordCount   number of coordinates in coords []
                \param[in] countTol     point count tolerance
                \param[out] keys        indicates for each polyline point if it is a key
            */
            static void ApproximateAlt (
                const value_type* coords,
                ptr_diff_type coordCount,
                unsigned countTol,
                unsigned char* keys)
            {
                ptr_diff_type pointCount = coordCount / DIM;
                // zero out keys
                memset (keys, 0, pointCount * sizeof (unsigned char));
                keys [0] = 1;                   // the first point is always a key
                keys [pointCount - 1] = 1;      // the last point is always a key
                unsigned keyCount = 2;

                if (countTol == 2) {
                    return;
                }

                typedef std::priority_queue <SubPolyAlt> PriorityQueue;
                PriorityQueue queue;    // sorted (max dist2) job queue containing sub-polylines

                SubPolyAlt subPoly (0, coordCount-DIM);
                subPoly.keyInfo = FindKey (coords, subPoly.first, subPoly.last);
                queue.push (subPoly);           // add complete poly

                while (!queue.empty ()) {
                    subPoly = queue.top ();     // take a sub poly
                    queue.pop ();
                    if (subPoly.keyInfo.index != subPoly.last) {
                        // store the key if valid
                        keys [subPoly.keyInfo.index / DIM] = 1;
                        // check point count tolerance
                        keyCount++;
                        if (keyCount == countTol) {
                            break;
                        }
                        // split the polyline at the key and recurse
                        SubPolyAlt left (subPoly.first, subPoly.keyInfo.index);
                        left.keyInfo = FindKey (coords, left.first, left.last);
                        queue.push (left);
                        SubPolyAlt right (subPoly.keyInfo.index, subPoly.last);
                        right.keyInfo = FindKey (coords, right.first, right.last);
                        queue.push (right);
                    }
                }
            }

        private:
            /*!
                \brief Finds the key for the given sub polyline.

                Finds the point in the range [first, last] that is furthest away from the
                segment (first, last). This point is called the key.

                \param[in] coords   array of polyline coordinates
                \param[in] first    the first coordinate of the first polyline point
                \param[in] last     the first coordinate of the last polyline point
                \return             the index of the key and its distance, or last when a key
                                    could not be found
            */
            static KeyInfo FindKey (
                const value_type* coords,
                ptr_diff_type first,
                ptr_diff_type last)
            {
                KeyInfo keyInfo;

                for (ptr_diff_type current = first + DIM; current < last; current += DIM) {
                    value_type d2 = math::segment_distance2 <DIM> (coords + first, coords + last,
                                                                   coords + current);
                    if (d2 < keyInfo.dist2) {
                        continue;
                    }
                    // update maximum squared distance and the point it belongs to
                    keyInfo.index = current;
                    keyInfo.dist2 = d2;
                }
                return keyInfo;
            }
        };
    };

    /*!
        \brief Performs the nth point routine (NP).

        This is a convenience function that provides template type deduction for
        PolylineSimplification::NthPoint.

        \param[in] first    the first coordinate of the first polyline point
        \param[in] last     one beyond the last coordinate of the last polyline point
        \param[in] n        specifies 'each nth point'
        \param[out] result  destination of the simplified polyline
        \return             one beyond the last coordinate of the simplified polyline
    */
    template <unsigned DIM, class InputIterator, class OutputIterator>
    OutputIterator simplify_nth_point (
        InputIterator first,
        InputIterator last,
        unsigned n,
        OutputIterator result)
    {
        PolylineSimplification <DIM, InputIterator, OutputIterator> ps;
        return ps.NthPoint (first, last, n, result);
    }

    /*!
        \brief Performs the (radial) distance between points routine (RD).

        This is a convenience function that provides template type deduction for
        PolylineSimplification::RadialDistance.

        \param[in] first    the first coordinate of the first polyline point
        \param[in] last     one beyond the last coordinate of the last polyline point
        \param[in] tol      radial (point-to-point) distance tolerance
        \param[out] result  destination of the simplified polyline
        \return             one beyond the last coordinate of the simplified polyline
    */
    template <unsigned DIM, class InputIterator, class OutputIterator>
    OutputIterator simplify_radial_distance (
        InputIterator first,
        InputIterator last,
        typename std::iterator_traits <InputIterator>::value_type tol,
        OutputIterator result)
    {
        PolylineSimplification <DIM, InputIterator, OutputIterator> ps;
        return ps.RadialDistance (first, last, tol, result);
    }

    /*!
        \brief Repeatedly performs the perpendicular distance routine (PD).

        This is a convenience function that provides template type deduction for
        PolylineSimplification::PerpendicularDistance.

        \param[in] first    the first coordinate of the first polyline point
        \param[in] last     one beyond the last coordinate of the last polyline point
        \param[in] tol      perpendicular (segment-to-point) distance tolerance
        \param[in] repeat   the number of times to successively apply the PD routine.
        \param[out] result  destination of the simplified polyline
        \return             one beyond the last coordinate of the simplified polyline
    */
    template <unsigned DIM, class InputIterator, class OutputIterator>
    OutputIterator simplify_perpendicular_distance (
        InputIterator first,
        InputIterator last,
        typename std::iterator_traits <InputIterator>::value_type tol,
        unsigned repeat,
        OutputIterator result)
    {
        PolylineSimplification <DIM, InputIterator, OutputIterator> ps;
        return ps.PerpendicularDistance (first, last, tol, repeat, result);
    }

    /*!
        \brief Performs the perpendicular distance routine (PD).

        This is a convenience function that provides template type deduction for
        PolylineSimplification::PerpendicularDistance.

        \param[in] first    the first coordinate of the first polyline point
        \param[in] last     one beyond the last coordinate of the last polyline point
        \param[in] tol      perpendicular (segment-to-point) distance tolerance
        \param[out] result  destination of the simplified polyline
        \return             one beyond the last coordinate of the simplified polyline
    */
    template <unsigned DIM, class InputIterator, class OutputIterator>
    OutputIterator simplify_perpendicular_distance (
        InputIterator first,
        InputIterator last,
        typename std::iterator_traits <InputIterator>::value_type tol,
        OutputIterator result)
    {
        PolylineSimplification <DIM, InputIterator, OutputIterator> ps;
        return ps.PerpendicularDistance (first, last, tol, result);
    }

    /*!
        \brief Performs Reumann-Witkam polyline simplification (RW).

        This is a convenience function that provides template type deduction for
        PolylineSimplification::ReumannWitkam.

        \param[in] first    the first coordinate of the first polyline point
        \param[in] last     one beyond the last coordinate of the last polyline point
        \param[in] tol      perpendicular (point-to-line) distance tolerance
        \param[out] result  destination of the simplified polyline
        \return             one beyond the last coordinate of the simplified polyline
    */
    template <unsigned DIM, class InputIterator, class OutputIterator>
    OutputIterator simplify_reumann_witkam (
        InputIterator first,
        InputIterator last,
        typename std::iterator_traits <InputIterator>::value_type tol,
        OutputIterator result)
    {
        PolylineSimplification <DIM, InputIterator, OutputIterator> ps;
        return ps.ReumannWitkam (first, last, tol, result);
    }

    /*!
        \brief Performs Douglas-Peucker polyline simplification (DP).

        This is a convenience function that provides template type deduction for
        PolylineSimplification::DouglasPeucker.

        \param[in] first    the first coordinate of the first polyline point
        \param[in] last     one beyond the last coordinate of the last polyline point
        \param[in] tol      perpendicular (point-to-segment) distance tolerance
        \param[out] result  destination of the simplified polyline
        \return             one beyond the last coordinate of the simplified polyline
    */
    template <unsigned DIM, class InputIterator, class OutputIterator>
    OutputIterator simplify_douglas_peucker (
        InputIterator first,
        InputIterator last,
        typename std::iterator_traits <InputIterator>::value_type tol,
        OutputIterator result)
    {
        PolylineSimplification <DIM, InputIterator, OutputIterator> ps;
        return ps.DouglasPeucker (first, last, tol, result);
    }

    /*!
        \brief Performs a variant of Douglas-Peucker polyline simplification (DPa).

        This is a convenience function that provides template type deduction for
        PolylineSimplification::DouglasPeuckerAlt.

        \param[in] first    the first coordinate of the first polyline point
        \param[in] last     one beyond the last coordinate of the last polyline point
        \param[in] count    the maximum number of points of the simplified polyline
        \param[out] result  destination of the simplified polyline
        \return             one beyond the last coordinate of the simplified polyline
    */
    template <unsigned DIM, class InputIterator, class OutputIterator>
    OutputIterator simplify_douglas_peucker_alt (
        InputIterator first,
        InputIterator last,
        unsigned count,
        OutputIterator result)
    {
        PolylineSimplification <DIM, InputIterator, OutputIterator> ps;
        return ps.DouglasPeuckerAlt (first, last, count, result);
    }
}

#endif // PSIMPL_GENERIC
