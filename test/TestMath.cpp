#include "TestMath.h"
#include "test.h"
#include "helper.h"
#include "../psimpl.h"
#include <vector>
#include <deque>
#include <list>
#include <set>
#include <limits>


namespace psimpl {
    namespace test
{

    // todo point dist, line dist -> check what happens when points are almost the same (within 1.5
    // epsilon of each other) and how the fraction calc can reach inf or nan
    TestMath::TestMath () {
        TEST_RUN("equal | random iterator", TestEqual_RandomIterator ());
        TEST_RUN("equal | bidirectional iterator", TestEqual_BidirectionalIterator ());
        TEST_DISABLED("equal | forward iterator", TestEqual_ForwardIterator ());

        TEST_RUN("make_vector | random iterator", TestMakeVector_RandomIterator ());
        TEST_RUN("make_vector | bidirectional iterator", TestMakeVector_BidirectionalIterator ());
        TEST_DISABLED("make_vector | forward iterator", TestMakeVector_ForwardIterator ());

        TEST_RUN("dot | random iterator", TestDot_RandomIterator ());
        TEST_RUN("dot | bidirectional iterator", TestDot_BidirectionalIterator ());
        TEST_DISABLED("dot | forward iterator", TestDot_ForwardIterator ());

        TEST_RUN("interpolate | fractions", TestInterpolate_Fractions ());
        TEST_RUN("interpolate | random iterator", TestInterpolate_RandomIterator ());
        TEST_RUN("interpolate | bidirectional iterator", TestInterpolate_BidirectionalIterator ());
        TEST_DISABLED("interpolate forward iterator", TestInterpolate_ForwardIterator ());

        TEST_RUN("point_distance2", TestPointDistance ());
        TEST_RUN("point_distance2 | random iterator", TestPointDistance_RandomIterator ());
        TEST_RUN("point_distance2 | bidirectional iterator", TestPointDistance_BidirectionalIterator ());
        TEST_DISABLED("point_distance2 forward iterator", TestPointDistance_ForwardIterator ());

        TEST_RUN("line_distance2", TestLineDistance ());
        TEST_RUN("line_distance2 | random iterator", TestLineDistance_RandomIterator ());
        TEST_RUN("line_distance2 | bidirectional iterator", TestLineDistance_BidirectionalIterator ());
        TEST_DISABLED("line_distance2 | forward iterator", TestLineDistance_ForwardIterator ());

        TEST_RUN("segment_distance2", TestSegmentDistance ());
        TEST_RUN("segment_distance2 | random iterator", TestSegmentDistance_RandomIterator ());
        TEST_RUN("segment_distance2 | bidirectional iterator", TestSegmentDistance_BidirectionalIterator ());
        TEST_DISABLED("segment_distance2 | forward iterator", TestSegmentDistance_ForwardIterator ());

        TEST_RUN("ray_distance2", TestRayDistance ());
        TEST_RUN("ray_distance2 | random iterator", TestRayDistance_RandomIterator ());
        TEST_RUN("ray_distance2 | bidirectional iterator", TestRayDistance_BidirectionalIterator ());
        TEST_DISABLED("ray_distance2 | forward iterator", TestRayDistance_ForwardIterator ());
    }

    void TestMath::TestEqual_RandomIterator () {
        {
            const unsigned dim = 2;
            double p1 [dim] = {1, 2};
            double p2 [dim] = {1, 2};
            double p3 [dim] = {2, 3};
            VERIFY_TRUE(psimpl::math::equal <dim> (p1, p2));
            VERIFY_FALSE(psimpl::math::equal <dim> (p1, p3));
        }
        {
            const unsigned dim = 3;
            float v [dim] = {1, 2, 3};
            float w [dim] = {2, 3, 4};
            std::vector <float> p1(v, v + dim);
            std::vector <float> p2(v, v + dim);
            std::vector <float> p3(w, w + dim);
            VERIFY_TRUE(psimpl::math::equal <dim> (p1.begin (), p2.begin ()));
            VERIFY_FALSE(psimpl::math::equal <dim> (p1.begin (), p3.begin ()));
        }
        {
            const unsigned dim = 4;
            int v [dim] = {1, 2, 3, 4};
            int w [dim] = {2, 3, 4, 5};
            std::deque <int> p1(v, v + dim);
            std::deque <int> p2(v, v + dim);
            std::deque <int> p3(w, w + dim);
            VERIFY_TRUE(psimpl::math::equal <dim> (p1.begin (), p2.begin ()));
            VERIFY_FALSE(psimpl::math::equal <dim> (p1.begin (), p3.begin ()));
        }
    }

    void TestMath::TestEqual_BidirectionalIterator () {
        {
            const unsigned dim = 2;
            float v [dim] = {1, 2};
            float w [dim] = {2, 3};
            std::list <float> p1(v, v + dim);
            std::list <float> p2(v, v + dim);
            std::list <float> p3(w, w + dim);
            VERIFY_TRUE(psimpl::math::equal <dim> (p1.begin (), p2.begin ()));
            VERIFY_FALSE(psimpl::math::equal <dim> (p1.begin (), p3.begin ()));
        }
        {
            const unsigned dim = 3;
            int v [dim] = {1, 2, 3};
            int w [dim] = {2, 3, 4};
            std::set <int> p1(v, v + dim);
            std::set <int> p2(v, v + dim);
            std::set <int> p3(w, w + dim);
            VERIFY_TRUE(psimpl::math::equal <dim> (p1.begin (), p2.begin ()));
            VERIFY_FALSE(psimpl::math::equal <dim> (p1.begin (), p3.begin ()));
        }
    }

    void TestMath::TestEqual_ForwardIterator () {
        // TODO
        ASSERT_TRUE(false);
    }

    void TestMath::TestMakeVector_RandomIterator () {
        {
            const unsigned dim = 2;
            double p1 [dim] = {1, 2};
            double p2 [dim] = {3, 5};
            double vec [dim] = {0, 0};
            psimpl::math::make_vector <dim> (p1, p2, vec);

            double expect [dim] = {2, 3};
            VERIFY_TRUE(ComparePoint <dim> (vec, expect));
        }
        {
            const unsigned dim = 3;
            float v [dim] = {1, 2, 3};
            float w [dim] = {4, 6, 8};
            std::vector <float> p1 (v, v + dim);
            std::vector <float> p2 (w, w + dim);
            std::vector <float> vec;
            psimpl::math::make_vector <dim> (p1.begin (), p2.begin (), std::back_inserter (vec));

            VERIFY_TRUE(vec.size () == dim);
            float e [dim] = {3, 4, 5};
            std::vector <float> expect (e, e + dim);
            VERIFY_TRUE(ComparePoint <dim> (vec.begin (), expect.begin ()));
        }
        {
            const unsigned dim = 4;
            int v [dim] = {1, 2, 3, 4};
            int w [dim] = {4, 6, 8, 10};
            std::deque <int> p1 (v, v + dim);
            std::deque <int> p2 (w, w + dim);
            std::deque <int> vec;
            psimpl::math::make_vector <dim> (p1.begin (), p2.begin (), std::back_inserter (vec));

            VERIFY_TRUE(vec.size () == dim);
            int e [dim] = {3, 4, 5, 6};
            std::deque <int> expect (e, e + dim);
            VERIFY_TRUE(ComparePoint <dim> (vec.begin (), expect.begin ()));
        }
    }

    void TestMath::TestMakeVector_BidirectionalIterator () {
        {
            const unsigned dim = 2;
            float v [dim] = {1, 2};
            float w [dim] = {3, 5};
            std::list <float> p1 (v, v + dim);
            std::list <float> p2 (w, w + dim);
            std::list <float> vec;
            psimpl::math::make_vector <dim> (p1.begin (), p2.begin (), std::back_inserter (vec));

            VERIFY_TRUE(vec.size () == dim);
            float e [dim] = {2, 3};
            std::list <float> expect (e, e + dim);
            VERIFY_TRUE(ComparePoint <dim> (vec.begin (), expect.begin ()));
        }
        {
            const unsigned dim = 4;
            int v [dim] = {1, 2, 3, 4};
            int w [dim] = {4, 6, 8, 10};
            std::set <int> p1 (v, v + dim);
            std::set <int> p2 (w, w + dim);
            std::set <int> vec;
            psimpl::math::make_vector <dim> (p1.begin (), p2.begin (), std::inserter (vec, vec.end ()));

            VERIFY_TRUE(vec.size () == dim);
            int e [dim] = {3, 4, 5, 6};
            std::set <int> expect (e, e + dim);
            VERIFY_TRUE(ComparePoint <dim> (vec.begin (), expect.begin ()));
        }
    }

    void TestMath::TestMakeVector_ForwardIterator () {
        // TODO
        ASSERT_TRUE(false);
    }

    void TestMath::TestDot_RandomIterator () {
        {
            const unsigned dim = 2;
            double p1 [dim] = {1, 2};
            double p2 [dim] = {2, 3};
            VERIFY_TRUE(psimpl::math::dot <dim> (p1, p2) == 8.0);
        }
        {
            const unsigned dim = 3;
            float v [dim] = {1, 2, 3};
            float w [dim] = {2, 3, 4};
            std::vector <float> p1(v, v + dim);
            std::vector <float> p2(w, w + dim);
            VERIFY_TRUE(psimpl::math::dot <dim> (p1.begin (), p2.begin ()) == 20.f);
        }
        {
            const unsigned dim = 4;
            int v [dim] = {1, 2, 3, 4};
            int w [dim] = {2, 3, 4, 5};
            std::deque <int> p1(v, v + dim);
            std::deque <int> p2(w, w + dim);
            VERIFY_TRUE(psimpl::math::dot <dim> (p1.begin (), p2.begin ()) == 40);
        }
    }

    void TestMath::TestDot_BidirectionalIterator () {
        {
            const unsigned dim = 2;
            float v [dim] = {1, 2};
            float w [dim] = {2, 3};
            std::list <float> p1(v, v + dim);
            std::list <float> p2(w, w + dim);
            VERIFY_TRUE(psimpl::math::dot <dim> (p1.begin (), p2.begin ()) == 8.f);
        }
        {
            const unsigned dim = 3;
            int v [dim] = {1, 2, 3};
            int w [dim] = {2, 3, 4};
            std::set <int> p1(v, v + dim);
            std::set <int> p2(w, w + dim);
            VERIFY_TRUE(psimpl::math::dot <dim> (p1.begin (), p2.begin ()) == 20);
        }
    }

    void TestMath::TestDot_ForwardIterator () {
        // TODO
        ASSERT_TRUE(false);
    }


    void TestMath::TestInterpolate_Fractions () {
        const unsigned dim = 2;
        float p1 [dim] = {1, 2};
        float p2 [dim] = {2, 3};
        float p3 [dim];
        {
            float fraction = -1.f;
            psimpl::math::interpolate <dim> (p1, p2, fraction, p3);
            float expect [dim] = {0.f, 1.f};
            VERIFY_TRUE(ComparePoint <dim> (p3, expect));
        }
        {
            float fraction = 0.f;
            psimpl::math::interpolate <dim> (p1, p2, fraction, p3);
            VERIFY_TRUE(ComparePoint <dim> (p3, p1));
        }
        {
            float fraction = 1.f;
            psimpl::math::interpolate <dim> (p1, p2, fraction, p3);
            VERIFY_TRUE(ComparePoint <dim> (p3, p2));
        }
        {
            float fraction = 2.f;
            psimpl::math::interpolate <dim> (p1, p2, fraction, p3);
            float expect [dim] = {3.f, 4.f};
            VERIFY_TRUE(ComparePoint <dim> (p3, expect));
        }
    }

    void TestMath::TestInterpolate_RandomIterator () {
        {
            const unsigned dim = 2;
            double p1 [dim] = {1, 2};
            double p2 [dim] = {2, 3};
            double p3 [dim];
            float fraction = 0.1f;
            psimpl::math::interpolate <dim> (p1, p2, fraction, p3);

            double expect [dim] = {1.1, 2.1};
            VERIFY_TRUE(ComparePoint <dim> (p3, expect));
        }
        {
            const unsigned dim = 3;
            float v [dim] = {1, 2, 3};
            float w [dim] = {2, 3, 4};
            std::vector <float> p1(v, v + dim);
            std::vector <float> p2(w, w + dim);
            std::vector <float> p3;
            float fraction = 0.1f;
            psimpl::math::interpolate <dim> (p1.begin (), p2.begin (), fraction, std::back_inserter (p3));
            
            float e [dim] = {1.1f, 2.1f, 3.1f};
            std::vector <float> expect (e, e + dim);
            VERIFY_TRUE(ComparePoint <dim> (p3.begin (), expect.begin ()));
        }
        {
            const unsigned dim = 4;
            int v [dim] = {1, 2, 3, 4};
            int w [dim] = {4, 6, 8, 10};
            std::deque <int> p1(v, v + dim);
            std::deque <int> p2(w, w + dim);
            std::deque <int> p3;
            float fraction = 0.5f;
            psimpl::math::interpolate <dim> (p1.begin (), p2.begin (), fraction, std::back_inserter (p3));
            
            int e [dim] = {2, 4, 5, 7};
            std::deque <int> expect (e, e + dim);
            VERIFY_TRUE(ComparePoint <dim> (p3.begin (), expect.begin ()));
        }
    }

    void TestMath::TestInterpolate_BidirectionalIterator () {
        {
            const unsigned dim = 3;
            float v [dim] = {1, 2, 3};
            float w [dim] = {2, 3, 4};
            std::list <float> p1(v, v + dim);
            std::list <float> p2(w, w + dim);
            std::list <float> p3;
            float fraction = 0.9f;
            psimpl::math::interpolate <dim> (p1.begin (), p2.begin (), fraction, std::back_inserter (p3));
            
            float e [dim] = {1.9f, 2.9f, 3.9f};
            std::list <float> expect (e, e + dim);
            VERIFY_TRUE(ComparePoint <dim> (p3.begin (), expect.begin ()));
        }
        {
            const unsigned dim = 4;
            int v [dim] = {1, 2, 3, 4};
            int w [dim] = {4, 6, 8, 10};
            std::set <int> p1(v, v + dim);
            std::set <int> p2(w, w + dim);
            std::set <int> p3;
            float fraction = 0.5f;
            psimpl::math::interpolate <dim> (p1.begin (), p2.begin (), fraction, std::inserter (p3, p3.end ()));
            
            int e [dim] = {2, 4, 5, 7};
            std::set <int> expect (e, e + dim);
            VERIFY_TRUE(ComparePoint <dim>(p3.begin (), expect.begin ()));
        }
    }

    void TestMath::TestInterpolate_ForwardIterator () {
        // TODO
        ASSERT_TRUE(false);
    }

    void TestMath::TestPointDistance () {
        const unsigned dim = 2;
        // identical points
        {
            float p1 [] = {1.f, 3.f};
            float p2 [] = {1.f, 3.f};
            VERIFY_TRUE(CompareValue(0.f, psimpl::math::point_distance2 <dim> (p1, p2)));
        }
        // vertical difference
        {
            float p1 [] = {0.f, 1.f};
            float p2 [] = {0.f, 3.f};
            VERIFY_TRUE(CompareValue(4.f, psimpl::math::point_distance2 <dim> (p1, p2)));
        }
        // horizontal distance
        {
            float p1 [] = {2.f, 0.f};
            float p2 [] = {5.f, 0.f};
            VERIFY_TRUE(CompareValue(9.f, psimpl::math::point_distance2 <dim> (p1, p2)));
        }
    }

    void TestMath::TestPointDistance_RandomIterator () {
        {
            const unsigned dim = 2;
            float p1 [] = {1.f, 3.f};
            float p2 [] = {2.f, 5.f};
            VERIFY_TRUE(CompareValue(5.f, psimpl::math::point_distance2 <dim> (p1, p2)));
        }
        {
            const unsigned dim = 3;
            double v [] = {1.0, 3.0, 4.0};
            double w [] = {2.0, 5.0, 7.0};
            std::vector <double> p1 (v, v + dim);
            std::vector <double> p2 (w, w + dim);
            VERIFY_TRUE(CompareValue(14.0, psimpl::math::point_distance2 <dim> (p1.begin (), p2.begin ())));
        }
        {
            const unsigned dim = 4;
            int v [] = {1, 3, 4, 6};
            int w [] = {2, 5, 7, 8};
            std::deque <int> p1 (v, v + dim);
            std::deque <int> p2 (w, w + dim);
            VERIFY_TRUE(CompareValue(18, psimpl::math::point_distance2 <dim> (p1.begin (), p2.begin ())));
        }
    }

    void TestMath::TestPointDistance_BidirectionalIterator () {
        {
            const unsigned dim = 2;
            float v [] = {1.f, 3.f};
            float w [] = {2.f, 5.f};
            std::list <float> p1 (v, v + dim);
            std::list <float> p2 (w, w + dim);
            VERIFY_TRUE(CompareValue(5.f, psimpl::math::point_distance2 <dim> (p1.begin (), p2.begin ())));
        }
        {
            const unsigned dim = 3;
            double v [] = {1.0, 3.0, 4.0};
            double w [] = {2.0, 5.0, 7.0};
            std::set <double> p1 (v, v + dim);
            std::set <double> p2 (w, w + dim);
            VERIFY_TRUE(CompareValue(14.0, psimpl::math::point_distance2 <dim> (p1.begin (), p2.begin ())));
        }
    }

    void TestMath::TestPointDistance_ForwardIterator () {
        ASSERT_TRUE(false); // TODO
    }


    void TestMath::TestLineDistance () {
        const unsigned dim = 2;
        // zero length line
        {
            float l1 [] = {0.f, 0.f};
            float l2 [] = {0.f, 0.f};
            float p1 [] = {0.f, 0.f};
            VERIFY_TRUE(CompareValue(0.f, psimpl::math::line_distance2 <dim> (l1, l2, p1)));
            float p2 [] = {4.f, 3.f};
            VERIFY_TRUE(CompareValue(25.f, psimpl::math::line_distance2 <dim> (l1, l2, p2)));
        }
        // tiny line
        {
            float l1 [] = {0.f, 0.f};
            float l2 [] = {0.f, std::numeric_limits <float>::min ()};
            float p1 [] = {std::numeric_limits <float>::min (), 0.f};
            VERIFY_TRUE(CompareValue(0.f, psimpl::math::line_distance2 <dim> (l1, l2, p1)));
            float p2 [] = {4.f, 3.f};
            VERIFY_TRUE(CompareValue(25.f, psimpl::math::line_distance2 <dim> (l1, l2, p2)));
        }
        // end points
        {
            float l1 [] = {4.f, 2.f};
            float l2 [] = {1.f, 7.f};
            float p1 [] = {4.f, 2.f};    // start
            VERIFY_TRUE(CompareValue(0.f, psimpl::math::line_distance2 <dim> (l1, l2, p1)));
            float p2 [] = {1.f, 7.f};    // end
            VERIFY_TRUE(CompareValue(0.f, psimpl::math::line_distance2 <dim> (l1, l2, p2)));
        }
        // vertical line
        {
            float l1 [] = {0.f, 1.f};
            float l2 [] = {0.f, 3.f};
            float p1 [] = {0.f, 0.f};    // below
            VERIFY_TRUE(CompareValue(0.f, psimpl::math::line_distance2 <dim> (l1, l2, p1)));
            float p2 [] = {0.f, 9.f};    // above
            VERIFY_TRUE(CompareValue(0.f, psimpl::math::line_distance2 <dim> (l1, l2, p2)));
            float p3 [] = {10.f, -6.f};    // to the right
            VERIFY_TRUE(CompareValue(100.f, psimpl::math::line_distance2 <dim> (l1, l2, p3)));
            float p4 [] = {-4.f, 8.f};    // to the left
            VERIFY_TRUE(CompareValue(16.f, psimpl::math::line_distance2 <dim> (l1, l2, p4)));
        }
        // horizontal line
        {
            float l1 [] = {2.f, 0.f};
            float l2 [] = {3.f, 0.f};
            float p1 [] = {1.f, 0.f};    // to the left
            VERIFY_TRUE(CompareValue(0.f, psimpl::math::line_distance2 <dim> (l1, l2, p1)));
            float p2 [] = {7.f, 0.f};    // to the left
            VERIFY_TRUE(CompareValue(0.f, psimpl::math::line_distance2 <dim> (l1, l2, p2)));
            float p3 [] = {9.f, 2.f};    // above
            VERIFY_TRUE(CompareValue(4.f, psimpl::math::line_distance2 <dim> (l1, l2, p3)));
            float p4 [] = {4.f, -3.f};    // below
            VERIFY_TRUE(CompareValue(9.f, psimpl::math::line_distance2 <dim> (l1, l2, p4)));
        }
    }

    void TestMath::TestLineDistance_RandomIterator () {
        {
            const unsigned dim = 2;
            float l1 [] = {1.f, 1.f};
            float l2 [] = {2.f, 2.f};
            float p [] = {1.f, 3.f};
            VERIFY_TRUE(CompareValue(2.f, psimpl::math::line_distance2 <dim> (l1, l2, p)));
        }
        {
            const unsigned dim = 3;
            double v [] = {1.0, 0.0, 1.0};
            double w [] = {2.0, 0.0, 2.0};
            double u [] = {1.0, 0.0, 3.0};
            std::vector <double> l1 (v, v + dim);
            std::vector <double> l2 (w, w + dim);
            std::vector <double> p (u, u + dim);
            VERIFY_TRUE(CompareValue(2.0, psimpl::math::line_distance2 <dim> (l1.begin (), l2.begin (), p.begin ())));
        }
        {
            const unsigned dim = 4;
            int v [] = {1, 0, 3, 0};
            int w [] = {0, 0, 0, 0};
            int u [] = {4, 0, 2, 0};
            std::deque  <int> l1 (v, v + dim);
            std::deque  <int> l2 (w, w + dim);
            std::deque  <int> p (u, u + dim);
            VERIFY_TRUE(CompareValue(10, psimpl::math::line_distance2 <dim> (l1.begin (), l2.begin (), p.begin ())));
        }
    }

    void TestMath::TestLineDistance_BidirectionalIterator () {
        {
            const unsigned dim = 2;
            float v [] = {1.f, 1.f};
            float w [] = {2.f, 2.f};
            float u [] = {1.f, 3.f};
            std::list <float> l1 (v, v + dim);
            std::list <float> l2 (w, w + dim);
            std::list <float> p (u, u + dim);
            VERIFY_TRUE(CompareValue(2.f, psimpl::math::line_distance2 <dim> (l1.begin (), l2.begin (), p.begin ())));
        }
        {
            const unsigned dim = 3;
            double v [] = {1.0, 0.0, 2.0};
            double w [] = {2.0, 0.0, 3.0};
            double u [] = {1.0, 0.0, 4.0};
            std::set <double> l1 (v, v + dim);
            std::set <double> l2 (w, w + dim);
            std::set <double> p (u, u + dim);
            VERIFY_TRUE(CompareValue(2.0, psimpl::math::line_distance2 <dim> (l1.begin (), l2.begin (), p.begin ())));
        }
    }

    void TestMath::TestLineDistance_ForwardIterator () {
        ASSERT_TRUE(false); // TODO
    }

    void TestMath::TestSegmentDistance () {
        const unsigned dim = 2;
        // zero length segment
        {
            float l1 [] = {0.f, 0.f};
            float l2 [] = {0.f, 0.f};
            float p1 [] = {0.f, 0.f};
            VERIFY_TRUE(CompareValue(0.f, psimpl::math::segment_distance2 <dim> (l1, l2, p1)));
            float p2 [] = {4.f, 3.f};
            VERIFY_TRUE(CompareValue(25.f, psimpl::math::segment_distance2 <dim> (l1, l2, p2)));
        }
        // tiny segment
        {
            float l1 [] = {0.f, 0.f};
            float l2 [] = {0.f, std::numeric_limits <float>::min ()};
            float p1 [] = {std::numeric_limits <float>::min (), 0.f};
            VERIFY_TRUE(CompareValue(0.f, psimpl::math::segment_distance2 <dim> (l1, l2, p1)));
            float p2 [] = {4.f, 3.f};
            VERIFY_TRUE(CompareValue(25.f, psimpl::math::segment_distance2 <dim> (l1, l2, p2)));
        }
        // end points
        {
            float l1 [] = {4.f, 2.f};
            float l2 [] = {1.f, 7.f};
            float p1 [] = {4.f, 2.f};    // start
            VERIFY_TRUE(CompareValue(0.f, psimpl::math::segment_distance2 <dim> (l1, l2, p1)));
            float p2 [] = {1.f, 7.f};    // end
            VERIFY_TRUE(CompareValue(0.f, psimpl::math::segment_distance2 <dim> (l1, l2, p2)));
        }
        // vertical segment
        {
            float l1 [] = {0.f, 1.f};
            float l2 [] = {0.f, 3.f};
            float p1 [] = {0.f, 0.f};    // below
            VERIFY_TRUE(CompareValue(1.f, psimpl::math::segment_distance2 <dim> (l1, l2, p1)));
            float p2 [] = {0.f, 9.f};    // above
            VERIFY_TRUE(CompareValue(36.f, psimpl::math::segment_distance2 <dim> (l1, l2, p2)));
            float p3 [] = {10.f, 2.f};   // to the right
            VERIFY_TRUE(CompareValue(100.f, psimpl::math::segment_distance2 <dim> (l1, l2, p3)));
            float p4 [] = {-4.f, 2.f};   // to the left
            VERIFY_TRUE(CompareValue(16.f, psimpl::math::segment_distance2 <dim> (l1, l2, p4)));

            float p5 [] = {2.f, 0.f};    // below right
            VERIFY_TRUE(CompareValue(5.f, psimpl::math::segment_distance2 <dim> (l1, l2, p5)));
            float p6 [] = {-2.f, 0.f};   // below left
            VERIFY_TRUE(CompareValue(5.f, psimpl::math::segment_distance2 <dim> (l1, l2, p6)));
            float p7 [] = {3.f, 5.f};    // above right
            VERIFY_TRUE(CompareValue(13.f, psimpl::math::segment_distance2 <dim> (l1, l2, p7)));
            float p8 [] = {-3.f, 5.f};   // above left
            VERIFY_TRUE(CompareValue(13.f, psimpl::math::segment_distance2 <dim> (l1, l2, p8)));
        }
        // horizontal segment
        {
            float l1 [] = {2.f, 0.f};
            float l2 [] = {5.f, 0.f};
            float p1 [] = {1.f, 0.f};    // to the left
            VERIFY_TRUE(CompareValue(1.f, psimpl::math::segment_distance2 <dim> (l1, l2, p1)));
            float p2 [] = {7.f, 0.f};    // to the right
            VERIFY_TRUE(CompareValue(4.f, psimpl::math::segment_distance2 <dim> (l1, l2, p2)));
            float p3 [] = {4.f, 2.f};    // above
            VERIFY_TRUE(CompareValue(4.f, psimpl::math::segment_distance2 <dim> (l1, l2, p3)));
            float p4 [] = {3.f, -3.f};   // below
            VERIFY_TRUE(CompareValue(9.f, psimpl::math::segment_distance2 <dim> (l1, l2, p4)));

            float p5 [] = {1.f, 2.f};    // left above
            VERIFY_TRUE(CompareValue(5.f, psimpl::math::segment_distance2 <dim> (l1, l2, p5)));
            float p6 [] = {1.f, -2.f};   // left below
            VERIFY_TRUE(CompareValue(5.f, psimpl::math::segment_distance2 <dim> (l1, l2, p6)));
            float p7 [] = {7.f, 2.f};    // right above
            VERIFY_TRUE(CompareValue(8.f, psimpl::math::segment_distance2 <dim> (l1, l2, p7)));
            float p8 [] = {7.f, -2.f};   // right below
            VERIFY_TRUE(CompareValue(8.f, psimpl::math::segment_distance2 <dim> (l1, l2, p8)));
        }
    }

    void TestMath::TestSegmentDistance_RandomIterator () {
        {
            const unsigned dim = 2;
            float l1 [] = {1.f, 1.f};
            float l2 [] = {2.f, 2.f};
            float p [] = {1.f, 3.f};
            VERIFY_TRUE(CompareValue(2.f, psimpl::math::segment_distance2 <dim> (l1, l2, p)));
        }
        {
            const unsigned dim = 3;
            double v [] = {1.0, 0.0, 1.0};
            double w [] = {2.0, 0.0, 2.0};
            double u [] = {1.0, 0.0, 3.0};
            std::vector <double> l1 (v, v + dim);
            std::vector <double> l2 (w, w + dim);
            std::vector <double> p (u, u + dim);
            VERIFY_TRUE(CompareValue(2.0, psimpl::math::segment_distance2 <dim> (l1.begin (), l2.begin (), p.begin ())));
        }
        {
            const unsigned dim = 4;
            int v [] = {1, 0, 3, 0};
            int w [] = {0, 0, 0, 0};
            int u [] = {4, 0, 2, 0};
            std::deque  <int> l1 (v, v + dim);
            std::deque  <int> l2 (w, w + dim);
            std::deque  <int> p (u, u + dim);
            VERIFY_TRUE(CompareValue(10, psimpl::math::segment_distance2 <dim> (l1.begin (), l2.begin (), p.begin ())));
        }
    }

    void TestMath::TestSegmentDistance_BidirectionalIterator () {
        {
            const unsigned dim = 2;
            float v [] = {1.f, 1.f};
            float w [] = {2.f, 2.f};
            float u [] = {1.f, 3.f};
            std::list <float> l1 (v, v + dim);
            std::list <float> l2 (w, w + dim);
            std::list <float> p (u, u + dim);
            VERIFY_TRUE(CompareValue(2.f, psimpl::math::segment_distance2 <dim> (l1.begin (), l2.begin (), p.begin ())));
        }
        {
            const unsigned dim = 3;
            double v [] = {1.0, 0.0, 2.0};
            double w [] = {2.0, 0.0, 3.0};
            double u [] = {1.0, 0.0, 4.0};
            std::set <double> l1 (v, v + dim);
            std::set <double> l2 (w, w + dim);
            std::set <double> p (u, u + dim);
            VERIFY_TRUE(CompareValue(2.0, psimpl::math::segment_distance2 <dim> (l1.begin (), l2.begin (), p.begin ())));
        }
    }

    void TestMath::TestSegmentDistance_ForwardIterator () {
        ASSERT_TRUE(false); // TODO
    }

    void TestMath::TestRayDistance () {
    }

    void TestMath::TestRayDistance_RandomIterator () {
    }

    void TestMath::TestRayDistance_BidirectionalIterator () {
    }

    void TestMath::TestRayDistance_ForwardIterator () {
        ASSERT_TRUE(false); // TODO
    }

}}
