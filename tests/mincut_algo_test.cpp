/******************************************************************************
 * mincut_algo_test.h
 *
 * Source of VieCut.
 *
 ******************************************************************************
 * Copyright (C) 2018 Alexander Noe <alexander.noe@univie.ac.at>
 *
 * Published under the MIT license in the LICENSE file.
 *****************************************************************************/

#include <type_traits>
#ifdef PARALLEL
#include "algorithms/global_mincut/viecut.h"
#include "parallel/algorithm/exact_parallel_minimum_cut.h"
#else
#include "algorithms/global_mincut/cactus/cactus_mincut.h"
#include "algorithms/global_mincut/ks_minimum_cut.h"
#include "algorithms/global_mincut/matula_approx.h"
#include "algorithms/global_mincut/noi_minimum_cut.h"
#include "algorithms/global_mincut/padberg_rinaldi.h"
#include "algorithms/global_mincut/stoer_wagner_minimum_cut.h"
#include "algorithms/global_mincut/viecut.h"
#endif
#include "gtest/gtest.h"
#include "io/graph_io.h"

template <typename T>
class MincutAlgoTest : public testing::Test { };

#ifdef PARALLEL
typedef testing::Types<viecut<graphAccessPtr>,
                       exact_parallel_minimum_cut<graphAccessPtr> > MCAlgTypes;
#else
typedef testing::Types<viecut<graphAccessPtr>,
                       noi_minimum_cut<graphAccessPtr>,
                       padberg_rinaldi<graphAccessPtr>,
                       matula_approx<graphAccessPtr>,
                       ks_minimum_cut,
                       cactus_mincut<graphAccessPtr> > MCAlgTypes;
#endif

TYPED_TEST_CASE(MincutAlgoTest, MCAlgTypes);

TYPED_TEST(MincutAlgoTest, NoGraph) {
    graphAccessPtr G;
    TypeParam mc;

    EdgeWeight cut = mc.perform_minimum_cut(G);
    ASSERT_EQ(cut, (EdgeWeight) - 1);
}

TYPED_TEST(MincutAlgoTest, EmptyGraph) {
    graphAccessPtr G = std::make_shared<graph_access>();
    TypeParam mc;

    EdgeWeight cut = mc.perform_minimum_cut(G);
    ASSERT_EQ(cut, (EdgeWeight) - 1);
}

TYPED_TEST(MincutAlgoTest, UnweightedGraphFromFile) {
    graphAccessPtr G = graph_io::readGraphWeighted(
        std::string(VIECUT_PATH) + "/graphs/small.metis");
    TypeParam mc;
    EdgeWeight cut = mc.perform_minimum_cut(G);

#ifdef PARALLEL
    if (std::is_same<TypeParam,
                     exact_parallel_minimum_cut<graphAccessPtr> >::value ||
        std::is_same<TypeParam,
                     exact_parallel_minimum_cut<mutableGraphPtr> >::value) {
#else
    if (std::is_same<TypeParam, noi_minimum_cut<graphAccessPtr> >::value ||
        std::is_same<TypeParam, noi_minimum_cut<mutableGraphPtr> >::value) {
#endif
        ASSERT_EQ(cut, 2);
    } else {
        // inexact, we can only guarantee that minimum cut is
        // between minimum degree and minimum cut
        ASSERT_LE(cut, 3);
        ASSERT_GE(cut, 2);
    }
}

TYPED_TEST(MincutAlgoTest, WeightedGraphFromFile) {
    graphAccessPtr G = graph_io::readGraphWeighted(
        std::string(VIECUT_PATH) + "/graphs/small-wgt.metis");
    TypeParam mc;
    EdgeWeight cut = mc.perform_minimum_cut(G);

#ifdef PARALLEL
    if (std::is_same<TypeParam,
                     exact_parallel_minimum_cut<graphAccessPtr> >::value ||
        std::is_same<TypeParam,
                     exact_parallel_minimum_cut<mutableGraphPtr> >::value) {
#else
    if (std::is_same<TypeParam, noi_minimum_cut<graphAccessPtr> >::value ||
        std::is_same<TypeParam, noi_minimum_cut<mutableGraphPtr> >::value) {
#endif
        ASSERT_EQ(cut, 3);
    } else {
        // inexact, we can only guarantee that minimum cut is
        // between minimum degree and minimum cut
        ASSERT_LE(cut, 10);
        ASSERT_GE(cut, 3);
    }
}
